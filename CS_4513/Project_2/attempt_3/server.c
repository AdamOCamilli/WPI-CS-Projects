/* A server which enables a distributed shell for users on a Linux computer.
 *
 * This program creates a socket and binds it to 127.0.0.1:8888 . Clients can then create their own socket, and so
 * connect from separate terminals on this computer to 127.0.0.1.
 * They may then chat with each other, being able to print a list of currently connected users or disconnect at anytime.
 *
 * Author: Adam Camilli (aocamilli@wpi.edu), 6/20/17
 */

#define _XOPEN_SOURCE 700 // Needed to get crypt()

#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <unistd.h>    
#include <pthread.h> 
#include <time.h>
#include <errno.h>
#include <signal.h> // To catch SIGINT which is only way to stop server
#include <assert.h>

#define MAX_MSG_LEN 20000
#define MAX_NAME_LEN 200
#define MAX_USERS 100
#define DEFAULT_PORT 3390 // Default
#define IP "130.215.214.29" 

typedef struct user_t {
  char *name;         // User's name
  long salt;           // Random int unique to each user
  char *key;          // User's hash(passwd + salt)
  int socket;         // Socket file descriptor (mutable)
} user_t;

user_t *usertable[MAX_USERS];

user_t* search_by_key(user_t **table, char *query) {
  for (int i = 0; i < MAX_USERS; i++) {
    if (table[i] != NULL) {
      if (strncmp(table[i]->key, query, sizeof(table[i]->key)) == 0) {
	return table[i];
      }
    }
  } return NULL;
}

user_t* search_by_salt(user_t **table, int query) {
  for (int i = 0; i < MAX_USERS; i++) {
    if (table[i] != NULL) {
      if (table[i]->salt == query) {
	return table[i];
      }
    }
    
  } return NULL;
}

user_t* search_by_socket(user_t **table, int query) {
  for (int i = 0; i < MAX_USERS; i++) {
    if (table[i] != NULL) {
      if (table[i]->socket == query) {
	return table[i];
      }
    }
    
  } return NULL;
}

user_t* search_by_name(user_t **table, char *query) {
  for (int i = 0; i < MAX_USERS; i++) {
    if (table[i] != NULL) {
      if (strcmp(table[i]->name,query) == 0) {
	return table[i];
      }
    }    
  } return NULL;
}

int add_user_nopasswd(user_t **table, char *name, char *key, long salt) {

  if (table[MAX_USERS - 1] != NULL) return -1;
  
  user_t *user = malloc(sizeof(user));
  user->name = malloc(MAX_MSG_LEN);
  user->key = malloc(MAX_MSG_LEN);
  // Essential to duplicate pointer since name pointer will not be static
  strncpy(user->name, name, MAX_MSG_LEN);
  strncpy(user->key, key, MAX_MSG_LEN);
  user->salt = salt;
  	    
  int i = 0;
  for (; i < MAX_USERS; i++) {
    if (table[i] == NULL) {
      table[i] = user;      
      break;
    }
  } if (table[i] == NULL) 
      return -1;

  return 0;
}

int add_user(user_t **table, char *name, char *passwd) {
  
  if (table[MAX_USERS - 1] != NULL) return -1;
  
  user_t *user = malloc(sizeof(user));
  user->name = malloc(MAX_MSG_LEN);
  user->key = malloc(MAX_MSG_LEN);
  // Essential to duplicate pointer since name pointer will not be static
  strncpy(user->name, name, MAX_MSG_LEN);
  user->salt = rand();
  char *temp_salt = malloc(MAX_MSG_LEN);
  sprintf(temp_salt,"%ld",user->salt);
  //printf("Encrypting with salt %s\n",temp);
  strncpy(user->key, crypt(passwd, temp_salt), MAX_MSG_LEN);
  printf("\nHash added: %s\n",user->key);
		    
  int i = 0;
  for (; i < MAX_USERS; i++) {
    if (table[i] == NULL) {
      table[i] = user;      
      break;
    }
  } if (table[i] == NULL) 
      return -1;

  FILE *USERFP = fopen("usertable.txt","a+");
  if (USERFP == NULL) {
    perror("Failed to open file");
    exit(1);
  } 
  printf("Accessed user file\n");
  fprintf(USERFP, "%s %s %s\n", usertable[i]->name, usertable[i]->key, temp_salt);
  fclose(USERFP);

  free(temp_salt);
  return 0;
}

int delete_user(user_t **table, char *name) {
  // Don't try to delete non-existent user
  if (search_by_name(table, name) == NULL)
    return -1;
  
  for (int i = 0; i < MAX_USERS; i++) {
    if (table[i] != NULL) {
      if (strcmp(table[i]->name,name) == 0) {
	table[i] = NULL;
	return 0;
      }
    }
  } return -1;
  
}

/* Helper functions for clients*/
void client_handler(int csock, int ssock);
int send_all(int sockfd, const void *buf, size_t len);
int recv_all(int sockfd, void *buf, size_t size); 
void recv_string_size(int sockfd, int *size);
void send_string_size(int sockfd, int *p, int size);

/* Code to catch SIGINT which is only way to stop server */
static volatile int run = 1;
void int_handler(int dummy) {
  delete_user(usertable,"user");
  delete_user(usertable,"aocamilli");
  run = 0;
  return;
}

int main(int argc, char **argv) {

  // Seed differently each time
  srand(time(NULL));

  // Parse command line opts
  char ch;
  int user_port = -1;
  char *temp_user = malloc(MAX_MSG_LEN);
  char *temp_passwd  = malloc(MAX_MSG_LEN);
  while ((ch = getopt(argc, argv, "hp:u")) != -1) {
    switch(ch) {
    case 'h':
      printf("Usage: \n\t./server\t\t(default port 3390)\n");
      printf("\t./server -p <portnum>\t(use different port)\n");      
      printf("\t./server -h\t\t(display usage options)\n");       
      printf("\t./server -u\t\t(add new user)\n"); 
      return 0;
    case 'p':
      user_port = atoi(optarg);
      break;
    case 'u':
      printf("Name: ");
      fgets(temp_user, MAX_MSG_LEN, stdin);
      strtok(temp_user,"\n");
      printf("Password: ");
      system("stty -echo"); // Disable echo
      fgets(temp_passwd, MAX_MSG_LEN, stdin);
      system("stty echo"); // Renable echo
      strtok(temp_passwd,"\n");
      add_user(usertable, temp_user, temp_passwd);
      return 0;
    }
  }
  
  free(temp_user);
  free(temp_passwd);

  char *temp = malloc(MAX_MSG_LEN);
  char *args[3];
  FILE *USERFP = fopen("usertable.txt","r");
  if (USERFP == NULL) {
    printf("No users. Use 'server -u' to add yourself\n");
    return 1;
  }
  while(fgets(temp, MAX_MSG_LEN, USERFP) != NULL) {
    strtok(temp, " "); // Format of each line is "<name> <hash> <salt>"
    args[0] = temp;
    temp = strtok(NULL, " ");
    args[1] = temp;
    temp = strtok(NULL, " ");
    args[2] = temp;
    //printf("Args: %s %s %s\n",args[0], args[1], args[2]);
    add_user_nopasswd(usertable, args[0], args[1], atol(args[2]));
  }

  if (usertable[0] == NULL) {
    printf("No users. Use 'server -u' to add yourself\n");
    return 1;
  }

  // File descriptors for our various sockets
  int socket_desc, client_sock, c, *new_sock;
  // Socket address structures
  struct sockaddr_in server, client;

  // Create the socket
  // 1) Internet domain 2) Stream socket 3) Default protocol (TCP)
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1) {
    printf("Could not create socket\n");
  }
  // Enable reuse of socket
  int enable = 1;
  if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(1);
  }

  // Populate the sockaddr_in struct
  server.sin_family = AF_INET; // Internet
  server.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces
  if (user_port < 0)
    server.sin_port = DEFAULT_PORT; // Set default port number
  else 
    server.sin_port = user_port; // Set user defined port number

  // Bind address struct to socket
  if (bind(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0) {
    //print the error message
    perror("bind failed");
    return 1;
  }

  // Print server info
  char *dir_buf = malloc(MAX_MSG_LEN);
  getcwd(dir_buf,MAX_MSG_LEN);
  printf("Server created: \n\tPort: \t%d\n\tDir: \t%s\n", server.sin_port, dir_buf);
  free(dir_buf);

  // Now begin listening for clients
  listen(socket_desc, MAX_USERS);
  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
  
  // Upon connecting with client, we fork to give each client its own dedicated child process
  time_t endwait;
  time_t start = time(NULL);
  time_t seconds = 20;

  // Catch SIGINT so we don't have to kill process every time we Ctrl-C
  // which is the only way to exit server
  struct sigaction act;
  act.sa_handler = int_handler;
  sigaction(SIGINT, &act, NULL);
  while (run) {
    if ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
      //printf("Errno: %d\nNew Socket: %d\n",errno,client_sock); 
      printf("%d wants to connect\n", c);
      pid_t pid = fork();
      if (pid < 0) {
	perror("fork");
	exit(1);
      } else if (pid) { // Parent code
	waitpid(pid, NULL, 0); // wait for and reap each child process
	printf("Reaped child %d\n",pid);
      } else { // Child code
	new_sock = malloc(sizeof *new_sock);
	*new_sock = client_sock; 
	if (pid)
	  printf("Forked to child %d\n",pid);
	client_handler(*new_sock, socket_desc);
	close(socket_desc);
	free(new_sock);
	return 0;
      }
    }
  }
  
  printf("End of parent server\n");
  close(socket_desc);
  return 0;

}

void client_handler(int csock, int ssock) {
  
  int read_size; // How many bytes from recv
  int string_size; // Length of expected string from send()
  long long_reply;

  char client_message[MAX_MSG_LEN];
  char temp_message[MAX_MSG_LEN];
  char *name = malloc(MAX_NAME_LEN);

  // Get user's name and passwd hash
  while (1) {
    /* Verify user is registered */
    printf("Getting user's name...\n");
    memset(client_message, 0, MAX_MSG_LEN);
    recv_string_size(csock, &string_size); // Size of expected string
    read_size = recv_all(csock, client_message, string_size);
    if (read_size < 0) {
      perror("recv_all");
      printf("errno: %d\n",errno);
    } else if (read_size == 0) { // Client disconnected
      perror("Client not connected");
      close(csock);
      return;
    }
    name = strdup(client_message);
    printf("User's name: %s (%d bytes)\n",name, read_size);
    printf("Looking up %s...\n", name);
    // Check user is registered
    if (search_by_name(usertable,name) == NULL) {
      long_reply = -1;
      send(csock, &long_reply, sizeof (long *), 0); 
      continue; // Try again
    } else {
      long_reply = 0;
      send(csock, &long_reply, sizeof (long *), 0); 
      printf("%s found\n", name);
    }
    /* Verify user has correct hash */
    printf("Verifying user...\n");
    memset(client_message, 0, MAX_MSG_LEN);
    // Send user their salt
    long_reply = search_by_name(usertable,name)->salt;
    send(csock, &long_reply, sizeof (long *), 0);
    // Get size of user's calculated hash
    recv_string_size(csock, &string_size);
    read_size = recv_all(csock, client_message, string_size);
    if (strncmp(search_by_name(usertable,name)->key, client_message, MAX_NAME_LEN) == 0) {
      printf("Verification successful!\n");
      long_reply = 0;
      send(csock, &long_reply, sizeof (long *), 0);
      break;
    } else {
      printf("Verification failed\n");
      long_reply = 1;
      send(csock, &long_reply, sizeof (long *), 0);
    }

  }

  free(name);

}

/* recv() ALL bytes from socket */ 
int send_all(int sockfd, const void *buf, size_t len) {
  ssize_t n;
  const char *p = buf;

  while (len > 0) {
    n = send(sockfd, p, len, 0);
    if (n == -1) {
      return 1;
    }
    if (n <= 0)
      return 1;
    p += n;
    len -= n;
  }

  return 0;
}

/* recv() ALL bytes from socket and pick out ones we want 
*/ 
int recv_all(int sockfd, void *buf, size_t size) {
  int read_bytes = 0;
  int read = 0;
  
 while (read_bytes < size) {
   read += recv(sockfd, buf, size, 0);
   read_bytes += read;
   if (read < 0) {
     perror("recv_all");
     return read_bytes;
   } else if (read == 0) { // Server disconnected
     return read_bytes;
   }
 }

  return read_bytes;
}

void recv_string_size(int sockfd, int *size) {
  send(sockfd, size, sizeof (int *), 0);
  recv(sockfd, size, sizeof (int *), 0);
}

void send_string_size(int sockfd, int *p, int size) {
  recv(sockfd, p, sizeof (int *), 0);
  *p = size;
  send(sockfd, p, sizeof (int *), 0);
}
