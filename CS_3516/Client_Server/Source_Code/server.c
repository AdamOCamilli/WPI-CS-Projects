/* A server which functions as a chat program and coordinator for up to 20 users on a Linux computer.
 *
 * This program creates a socket and binds it to 127.0.0.1:8888 . Clients can then create their own socket, and so
 * connect from separate terminals on this computer to 127.0.0.1.
 * They may then chat with each other, being able to print a list of currently connected users or disconnect at anytime.
 *
 * Author: Adam Camilli (aocamilli@wpi.edu), 6/20/17
 */

#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>    
#include <pthread.h> 

#define MAX_MSG_LEN 1000
#define MAX_CLIENTS 20

/*******************************************************************************************
 **** Lookup table struct + accompanying functions and global vars for client nicknames ****
 *******************************************************************************************/
typedef struct pair {
  char* name;
  int socket;
} pair_t;

pair_t *nametable[MAX_CLIENTS];

pair_t* search_by_socket(pair_t **table, int query) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (table[i] != NULL) {
      if (table[i]->socket == query) {
	return table[i];
      }
    }
    
  } return NULL;
}
  
pair_t* search_by_name(pair_t **table, char *query) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (table[i] != NULL) {
      if (table[i]->name == query) {
	return table[i];
      }
    }    
  } return NULL;
}

int add_pair(pair_t **table, char *name, int socket) {

  if (table[MAX_CLIENTS - 1] != NULL) return -1;
  
  pair_t *pair = (pair_t *) malloc(sizeof(pair_t));
  // Essential to use duplicate pointer since name pointer will not be static
  pair->name = strdup(name);
  pair->socket = socket;

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (table[i] == NULL) {
      table[i] = pair;
      return 0;
    }
  } return -1;
  
}

int delete_pair(pair_t **table, char *name, int socket) {
  // Don't try to delete non-existent pair
  if (search_by_name(table, name) == NULL || search_by_socket(table,socket) == NULL)
    return -1;

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (table[i] != NULL) {
      if (table[i]->socket == socket && table[i]->name == name) {
	table[i] = NULL;
	return 0;
      }
    }
  } return -1;
  
}
	    
/**************************************************
 **** Thread Handler function and main program ****
 **************************************************/

void *connection_handler(void *);

int main(int argc , char *argv[]) {

  // Initialize nametable with null elements to mark it empty
  for (int i = 0; i < MAX_CLIENTS; i++) 
    nametable[i] = NULL;
  
  int socket_desc , client_sock , c , *new_sock;
  struct sockaddr_in server , client;
  
  // Create the socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
    {
      printf("Could not create socket\n");
    }
  puts("Socket created");
  
  // Populate the sockaddr_in struct
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY; // All available interfaces
  server.sin_port = htons( 8888 );
  
  // Bind it to all available interfaces
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
    //print the error message
    perror("bind failed. Error");
    return 1;
  }
  puts("bind done");
  
  // Now begin listening for up to clients
  listen(socket_desc, 20);
  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
  
  // Upon connecting with client provide them with a thread for concurrent communication with server
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
    
    // printf("Connection accepted from %d\n", client_sock);
    
    pthread_t sniffer_thread;
    new_sock = malloc(sizeof *new_sock);
    *new_sock = client_sock;
    
    if( pthread_create( &sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
      perror("could not create thread");
      return 1;
    }
    
    // Loop so that we don't terminate program
    // printf("Handler assigned to %d\n", client_sock);
  }
  
  if (client_sock < 0) {
    perror("accept failed");
    return 1;
  }
  
  return 0;
}

void *connection_handler(void *socket_desc) {

  int sock = *(int*)socket_desc; // Get the socket descriptor
  int read_size;
  char *message;
  char client_message[MAX_MSG_LEN];
  char *name;

  int named = 0; // Whether or not client has chosen a nickname
  int full_violation = 0; // Whether or not client tried to connect when server was full
  
  // Receive name and then messages from client
  while (((read_size = recv(sock, client_message, MAX_MSG_LEN, 0)) > 0)) {
    if (!named) {
      if (add_pair(nametable, client_message, sock) < 0) {
	char *full = "#full";
	write(sock, full, sizeof full);
	full_violation++;
	break;
      }

      write(sock, client_message, sizeof client_message);

      printf("%s has joined chat!\n", client_message);
      named = 1;
    } else {
      // Send client list of users if requested
      if (client_message[0] == '$') {
	char *userlist = (char *) malloc(MAX_MSG_LEN + 1);
	memset(userlist, 0, sizeof userlist);
        for (int j = 0; j < MAX_CLIENTS; j++) {
	  if (nametable[j] != NULL) {
	    char *temp = strcat(strdup(nametable[j]->name),"\n");
	    userlist = strcat(userlist,temp);	   
	  }
	}
	write(sock, userlist, sizeof userlist);
	free(userlist);	
      } else { // Always respond so client knows its message has been received
	write(sock, client_message, sizeof client_message);
	printf("%s: %s", search_by_socket(nametable,sock)->name, client_message);
	
      }
    }
  }

  // If client disconnects
  if (read_size == 0) {
      printf("%s disconnected\n", search_by_socket(nametable,sock)->name);
      fflush(stdout);
  } else if(read_size == -1) {
      perror("recv failed");
  }

  // Delete client name/socket entry upon exit
  if (!full_violation)
    delete_pair(nametable, search_by_socket(nametable,sock)->name, sock);
  // Free thread's socket pointer upon exit
  free(socket_desc);
  close(sock);
  pthread_exit(NULL); 
}

