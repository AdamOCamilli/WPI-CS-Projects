/* A client program which creates a socket with a server and communicates with it.
 *
 * Clients upon connecting with server 
 * Author: Adam Camilli (aocamilli@wpi.edu), 6/20/17
 */

#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h>

#define MAX_MSG_LEN 1000

/* Helper function to help remove non alphanumeric/space characters from a C-Style string */
int is_alphanumeric(char ch) {
  int not_uppercase = (ch < 'A' && ch > 'Z');
  int not_lowercase = (ch < 'a' && ch > 'z');
  int not_numeric = (ch < '1' && ch > '9');

  if (not_uppercase && not_lowercase && not_numeric)
    if (ch != ' ')
      return 0;
  
  return 1;
}

int main(int argc , char *argv[]) {
  // Whether we disconnected from server of our own accord
  int clean_break = 0;
  
  int sock;
  struct sockaddr_in server;
  char message[MAX_MSG_LEN] , server_reply[MAX_MSG_LEN];
  int named = 0;
  
  // Create socket our socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    printf("Could not create socket\n");
  } printf("Socket created\n");

  // If improper arguments given
  if (argc < 2 || argc > 2) {
    printf("Usage: ./client <IP Address>\n");
    return 1;
  }

  // Populate the sockaddr_in struct
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_family = AF_INET;
  server.sin_port = htons( 8888 );
  
  // Connect to server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
    perror("Connect failed. Server is not listening for clients.\n");
    return 1;
  }
  
  printf("Connected to %s\n", argv[1]);
  printf("Type '#' to end the session.\n");
  printf("Type '$' for a list of currently connected users.\n\n");

  // Continuously communicate with server
  while (1) {      
    if (!named) { // Name not chosen
      char name[MAX_MSG_LEN];
	printf("Enter your name (letters, numbers, and spaces only): ");
	fgets(name, sizeof name, stdin);

	if (name[0] == '$' || name[0] == '#' || name[0] == '@') {
	  printf("Must choose a name first.\n");
	  continue;
	}
	
	// Clear any non A-Z, a-z, 0-9 characters from name
	int i;
	for (i = 0; (name[i] != '\n') && i < MAX_MSG_LEN; i++) {
	  if (is_alphanumeric(name[i]))
	    message[i] = name[i];	      	    
	}
	named = 1;
      } else if (named) {	
	printf("Now type your message: ");
	fgets(message, sizeof message - 1, stdin);
      }

      // Disconnect if requested to
      if (message[0] == '#') {
	printf("Goodbye!\n");
	clean_break = 1;
	break;
      }
      
      // Otherwise send message to server
      if (send(sock, message, sizeof message, 0) < 0) {
	puts("Send failed.");
	break;
      } 
      
      // Receive a reply from the server
      if (recv(sock, server_reply, MAX_MSG_LEN, 0) < 0) {
	puts("recv failed");
	break;
      } if (message[0] == '$') {
	printf("%s\n",server_reply);	 
      } else if (server_reply[0] == '#') {
	printf("Sorry, server is full!\n");
	clean_break = 1;
	break;
      }
  }
  
  close(sock);
  if (!clean_break)
    printf("Server lost: Relaunch client to connect to another. Goodbye!\n");
  return 0;
}
