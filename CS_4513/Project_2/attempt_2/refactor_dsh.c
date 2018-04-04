/* A client program which creates a socket with a server and communicates with it.
 *
 * Clients upon connecting with server 
 * Author: Adam Camilli (aocamilli@wpi.edu), 6/20/17
 */

#define _XOPEN_SOURCE 700 // Needed to get crypt()

#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_MSG_LEN 10000
#define LOGIN_TRIES 4

int main(int argc , char *argv[]) {
  int clean_break = 0;
  int sock;
  struct sockaddr_in server;
  int named = 0;
  const char s[2] = " ";
  char *p;
 // char *token = malloc(sizeof (char) * MAX_MSG_LEN);  
  //char **arguments = malloc(sizeof (char*) * MAX_MSG_LEN);
  //char **command = malloc(sizeof (char*) * MAX_MSG_LEN);
  //char **shellInputOptions = malloc(sizeof (char*) * MAX_MSG_LEN);

  // Create the socket
  // 1) Internet domain 2) Stream socket 3) Default protocol (TCP)
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    printf("Could not create socket\n");
  } else printf("Socket created\n");
  
  
  // Check that the user entered a command
  if (argc != 3){
    printf("Usage: ./client <ip> <port>\n");
    return -1;
  }

  // Populate the sockaddr_in struct
  server.sin_addr.s_addr = inet_addr(argv[1]); // Given IP address
  server.sin_family = AF_INET;                 // Internet family
  server.sin_port = atoi(argv[2]);             // Port number
                                               
  /* Set all bits of the padding field to 0 */
  memset(server.sin_zero, '\0', sizeof server.sin_zero); 
  
  // Connect to server
  char *temp_arg = malloc(MAX_MSG_LEN);
  int changed = 0;
  while (connect(sock, (struct sockaddr *)&server , sizeof(server)) < 0) {
    printf("Connect failed. Server is not listening for clients.\n");
    // Get new port at runtime
    printf("Reenter port? (y/n):  ");
    if (getchar() == 'y') {
      changed = 1;	    
      getchar(); // Clear newline
      printf("New port: ");
      fgets(temp_arg, MAX_MSG_LEN, stdin);
      strtok(temp_arg, "\n"); // remove trailing newline
      server.sin_port = atoi(temp_arg);
    } else 
      getchar(); // Clear newline
    // Get new IP at runtime
    memset(temp_arg, 0, MAX_MSG_LEN); 
    printf("Renter IP? (y/n): ");
    if (getchar() == 'y') {
      changed = 1;
      getchar(); // Clear newline
      printf("New IP: ");
      fgets(temp_arg, MAX_MSG_LEN, stdin);
      strtok(temp_arg, "\n"); // remove trailing newline
      server.sin_addr.s_addr = inet_addr(temp_arg);
    } else if (!changed) {
      return 1;
    } else
      getchar();
  }
  free(temp_arg);
  printf("Successfully connected to %d\n", server.sin_port);
  fflush(stdout);

  char *userName  = malloc(MAX_MSG_LEN);
  char *server_reply = malloc(MAX_MSG_LEN);
  char *shellInput = malloc(MAX_MSG_LEN);
  char *temp_message = malloc(MAX_MSG_LEN);
  char *passwd = malloc(MAX_MSG_LEN);

  int verifying = 0;
  long long_server_reply, converted_reply;
  int tries = 5;
  while (tries >= 0) {
    if (!verifying) {
      if (tries < 5) 
	printf("%d tries left\n",tries);
      // Get user name
      memset(userName, 0, MAX_MSG_LEN);
      printf("Username: "); // Sent in plaintext to server
      fgets(userName, MAX_MSG_LEN, stdin);
      strtok(userName,"\n"); // Clear newline
      // Get password
      printf("Password: ");
      memset(passwd, 0, MAX_MSG_LEN);
      // Hide password
      system("stty -echo"); // Disable echo
      fgets(passwd, MAX_MSG_LEN, stdin); // Sent in hashed form to server
      system("stty echo"); // Reenable echo
      printf("\n");
      // Done 
      strtok(passwd,"\n"); // Clear newline
      if (send(sock, userName, MAX_MSG_LEN, 0) < 0) {
	puts("Send failed. Try again.\n");
	continue;
      }
      verifying = 1;
      continue;
    } else {
      int read_size = recv(sock, &long_server_reply, sizeof(int *), 0);
      if (read_size < 0) {
	puts("recv failed. Try again.");
	verifying = 0;
	continue;
      } else if (read_size == 0) {
	printf("Server disconnected\n");
	break;
      } else if (read_size > 0) {
	if (long_server_reply == -2) {
	  perror("Verification failed\n");
	  tries--;
	  verifying = 0;
	  continue;
	} else if (long_server_reply == -1) {
	  printf("No user %s found. Try again\n",userName);
	  verifying = 0;
	  tries--;
	  continue;
	}
	converted_reply = long_server_reply;
        if (converted_reply > 0) { // Server sent the salt value
	  printf("Encrypting with salt %ld\n", converted_reply);
	  snprintf(temp_message, MAX_MSG_LEN, "%ld", converted_reply);
	  // Send size of hash to server
	  int len = strlen(crypt(passwd,temp_message)) + 1; // +1 for null terminator
	  if (send(sock, &len, sizeof (int *), 0) < 0) {
	    perror("send");
	    continue;
	  }
	  // Let server acknowledge it has size of our hash
	  read_size = recv(sock, &long_server_reply, sizeof(int *), 0);
	  if (read_size < 0) {
	    puts("recv failed. Try again.");
	    verifying = 0;
	    continue;
	  } else if (read_size == 0) {
	    printf("Server disconnected\n");
	    break;
	  } else if (read_size > 0)
	    printf("Server: hash will be %ld bytes\n", long_server_reply);
	  
	  printf("Sending %s to server\n", crypt(passwd, temp_message));
	  if (send(sock, crypt(passwd, temp_message), len, 0) < 0) {
	    perror("send");
	    continue;
	  }
	  memset(temp_message, 0, MAX_MSG_LEN);
	  continue;
	} else if (converted_reply == 0) {
	  break; // Server has confirmed we are verified
	}
      }
    }
  }

  if (tries < 0) {
    printf("Failed to connect to server: too many failed attempts\n");
    return 1;
  }

  printf("Welcome to the remote shell %s!\n", userName);
  printf("To close the shell type: #\n");
  int ssize_known = 0; // The first two elements of the array are taken already
  // Will run the 'shell' until the user exits
  int disconnect = 0;
  while (1) {
    if (!ssize_known) {
      printf("> ");
      memset(server_reply, 0, sizeof(server_reply));
      memset(shellInput, 0, sizeof(shellInput));
      fgets(shellInput, MAX_MSG_LEN,stdin); // Clear newline
      strtok(shellInput,"\n");
      if (strncmp(shellInput, "#", sizeof(shellInput)) == 0) {
	clean_break = 1;
	disconnect = 1;
	printf("Goodbye, %s!\n",userName);
      }
      
      // Send size of msg to server
      int len = strlen(shellInput) + 1; // +1 for null terminator
      if (write(sock, &len, sizeof (int *)) < 0) {
	perror("send");
	continue;
    }
      // Let server acknowledge it has size of our msg
      int read_size = recv(sock, &len, sizeof(int *), 0);
      if (read_size < 0) {
	puts("recv failed. Try again.");
	verifying = 0;
	continue;
      } else if (read_size == 0) {
	printf("Server disconnected\n");
	break;
      } else if (read_size > 0)
	printf("Server: msg will be %d bytes\n", len);
      
      if (write(sock, shellInput, len) < 0) {
	puts("Send failed. Try again.\n");
	continue;
      } if (disconnect) {
	printf("Server disconnected normally\n");
	clean_break++;
	close(sock);
      break;
      }
      
      // Server will tell us length of command output
      read_size = recv(sock, &len, sizeof(int *), 0);
      if (read_size < 0) {
	puts("recv failed. Try again.");
	verifying = 0;
	continue;
      } else if (read_size == 0) {
	printf("Server disconnected\n");
	close(sock);
	break;
      } else if (read_size > 0) {
	printf("Server: output will be %d bytes\n", len);
	ssize_known = len;
	write(sock, &ssize_known, sizeof (int *));
	continue;
      }
    } else {
      memset(server_reply, 0, MAX_MSG_LEN);   
      if (recv(sock, server_reply, ssize_known, 0) < 0) {
	puts("recv failed. Try again.");
	continue;
      } else {
	printf("%s", server_reply);
      }
      ssize_known = 0;
    }
  }
  
  free(userName);
  free(passwd);
  free(server_reply);
  free(temp_message);
  free(shellInput);
  
  if (!clean_break) {
    printf("Server lost erroneously: May have to kill this process to launch again. Goodbye!\n");
    close(sock);
    return 1;
  }
  
  return 0;    
}
