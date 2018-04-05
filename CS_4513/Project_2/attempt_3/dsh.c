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

#define MAX_MSG_LEN 20000
#define MAX_NAME_LEN 200
#define LOGIN_TRIES 5
#define SIZE_QUERY 0

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

/* recv() ALL bytes from socket and pick out ones we want */ 
int recv_all(int sockfd, void *buf, size_t size) {
  int read_bytes = 0;
  int read = 0;
  
 while(read_bytes < size) {
    read += recv(sockfd, buf, size, 0);
    read_bytes += read;
    if (read < 0) {
      perror("recv");
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

int main(int argc , char *argv[]) {
  int clean_break = 0;
  int sock;
  struct sockaddr_in server;
  int named = 0;

  // Create the socket
  // 1) Internet domain 2) Stream socket 3) Default protocol (TCP)
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("Could not create socket");
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
  int changed_ip = 0;
  while (connect(sock, (struct sockaddr *)&server , sizeof(server)) < 0) {
    printf("Connect failed. Server is not listening for clients.\n");
    // Get new port at runtime
    printf("Reenter port? (y/n):  ");
    if (getchar() == 'y') {
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
      changed_ip = 1;
      getchar(); // Clear newline
      printf("New IP: ");
      fgets(temp_arg, MAX_MSG_LEN, stdin);
      strtok(temp_arg, "\n"); // remove trailing newline
      server.sin_addr.s_addr = inet_addr(temp_arg);;
    } else 
      return -1;    
  }
  
  if (!changed_ip) {
    printf("Successfully connected to %s\n", argv[1]);
  } else {
    printf("Successfully connected to %s\n", temp_arg);
  }
  free(temp_arg);

  char *username  = malloc(MAX_NAME_LEN);

  char *server_reply = malloc(MAX_MSG_LEN);
  char *shellInput = malloc(MAX_MSG_LEN);
  char *temp_message = malloc(MAX_MSG_LEN);
  char *passwd = malloc(MAX_MSG_LEN);

  int int_server_reply;
  int reply_size;
  int tries = 5;
  while (tries >= 0) {
    if (tries < 5) 
      printf("%d tries left\n",tries);
    // Get user name
    memset(username, 0, MAX_NAME_LEN);
    printf("Username: "); // Sent in plaintext to server
    fgets(username, MAX_NAME_LEN, stdin);
    strtok(username,"\n"); // Clear newline
    
    reply_size = strlen(username) + 1; // +1 for null terminator
    send_string_size(sock, &int_server_reply, reply_size); 
    
    if (send_all(sock, username, reply_size) < 0) {
      perror("Send failed. Try again.");
    }
    memset(server_reply, 0, MAX_MSG_LEN);
    if (recv_all(sock, server_reply, MAX_NAME_LEN)) {
      printf("Server: %s\n",server_reply);
      clean_break++;
      break;
    }
    tries--;
  }
  
  free(username);
  free(passwd);
  free(server_reply);
  free(temp_message);
  free(shellInput);
  
  if (!clean_break) {
    printf("Server lost erroneously: May have to kill this process to launch again. Goodbye!\n");
    close(sock);
    return 1;
  } else {
    printf("Server disconnected normally\n");
    close(sock);
  }
  
  return 0;    
}
