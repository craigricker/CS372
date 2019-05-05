/*
   ProgramName:        client.c
   Author:             Craig Ricker
   Course Name:        CS372 Intro to Computer Networks
   Last Modified:      May 5th, 2019
   Description:

   This program acts as the client end of a simple two way chat program.
   Utilizing sockets API, it creates a connection to a host and port.
   The host and port are passed in as argument 1, and arg 2 in the
   command line.
   Once chat is initiated, the users take turn sending messages to one another.
   If either user sends "\quit" the chat will be terminated.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>

#define DATASIZE 510
#define USERSIZE 10
#define QUIT_PROMPT "\\quit"
const char * PROMPT = "> ";


/*
 * clear_buffer
 * Clears buffer - largely adapted from
 * https://stackoverflow.com/questions/7898215/how-to-clear-input-buffer-in-c
 * Pre condition: None
 * Inputs: None
 * Returns: None
 * Post Conditions: buffer is cleared
 */
void clear_buffer() {
   int c;
   while ((c = getchar()) != '\n' && c != EOF) { }
}

/*
 * send_message
 * Prompts user, sends message and username through connection
 *
 * Pre condition: sockfd be active socket properly connected
 * Inputs:
 *    sockfd:     socket to send message to
 *    username:   username to include in prompt and message
 * Returns: T/F if user wishes to continue messaging
 * Post Conditions: Message is sent, could initiate quit protocol
 */
bool send_message(int sockfd, char * username) {
   char message[DATASIZE];
   char input[DATASIZE - 12];
   int bytes;
   // Clear buffers to prevent issues
   memset((char*)&message, '\0', sizeof(message));
   memset((char*)&input, '\0', sizeof(input));
   strcat(message, username);
   strcat(message, PROMPT);
   // Prompt user for message, format and send
   printf("%s", message);
   fgets(input, 500, stdin);
   strcat(message, input);
   // Drop the new line
   strtok(message, "\n");
   bytes = send(sockfd, message, strlen(message), 0);
   // Check message actually sent
   if (bytes == -1) {
      fprintf(stderr, "Failure to send data\n");
      exit(1);
   }
   // See if user wishes to quit
   if (strstr(message, QUIT_PROMPT) != NULL)
      return false;
   else
      return true;
}

/*
 * receive_message
 * listens to sockfd for incoming messages
 * Pre condition: sockfd must be active and properly connected
 *                socket
 * Inputs:
 *    sockfd: active socket connection
 * Returns: T/F if user wishes to continue conversation or not
 * Post Conditions: New message is printed to the screen
 */
bool receive_mesage(int sockfd) {
   char message[DATASIZE];
   int status;
   memset((char*)&message, '\0', sizeof(message));
   
   status = recv(sockfd, message, 500, 0);
   if (status == -1) {
      fprintf(stderr, "Failure to receive message.\n");
      exit(1);
   }
   if (strstr(message, QUIT_PROMPT) != NULL)
      return false;
   else {
      printf("%s\n", message);
      return true;
   }
}

/*
 * chat
 * Alternates sending and receiving messages between client and server
 * Pre condition: sockfd must be active and properly connected
 *                socket
 * Inputs:
 *    sockfd: active socket connection
 * Returns:
 * Post Conditions
 */
void chat(int sockfd, char * username) {
   while ((send_message(sockfd, username) &&
           receive_mesage(sockfd))) {}
}

/*
 * setup_address
 * Creates addrinfo object
 * Pre condition: address and port must both be valid, and computer
 *                have authority to connect to this socket
 * Inputs:
 *    address: host name to connect to
 *    port:    port number to connect to
 * Returns: addrinfo object created with address and port
 * Post Conditions
 */
struct addrinfo * setup_addres(char * address, char * port) {
   // Adopted from Beej's guide
   struct addrinfo hints, * res;
   int status;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   
   if ((status = getaddrinfo(address, port, &hints, &res)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
      exit(1);
   }
   return res;
}

/*
 * socket_setup
 * Creates and connects to socket based on addrinfo object
 * also sends username to initiate peer connection fully
 * Pre condition: res must be properly created addrinfo
 *                to a connection that is valid and accessible by this machine
 * Inputs:
 *    res:        connection to create socket to
 *    user_name:  Identifying tag of this user
 * Returns:       sockfd as an integer
 * Post Conditions: fully creates socket and returns value to
 *                   send and receive communications on.
 */
int socket_setup(struct addrinfo * res, char * user_name) {
   int sockfd, status, bytes;
   
   // Attempt socket creation
   if ((sockfd = socket(res->ai_family, res->ai_socktype,
                        res->ai_protocol)) == -1){
      fprintf(stderr, "Error. Socket creation failure.\n");
      exit(1);
   }
   
   // Attempt socket connection
   if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1){
      fprintf(stderr, "Error connecting to socket.\n");
      exit(1);
   }
   
   // Let server know user connected
   bytes = send(sockfd, user_name, strlen(user_name), 0);
   // Check message actually sent
   if (bytes == -1) {
      fprintf(stderr, "Failure to send data\n");
      exit(1);
   }
   return sockfd;
}


int main(int argc, char *argv[])
{
   char userName[USERSIZE];
   int sockfd;
   struct addrinfo * res;
   // Check command line arguments
   if (argc != 3) {
      fprintf(stderr, "Usage: chatclient server port\n");
      exit(-1);
   }
   // Read and store username
   memset((char*)&userName, '\0', sizeof(userName));
   printf("Please enter a username 10 characters or less:");
   scanf("%s", userName);
   clear_buffer();
   // Set up address
   res = setup_addres(argv[1], argv[2]);
   sockfd = socket_setup(res, userName);
   // Actually chat
   chat(sockfd, userName);
   // Upon exiting close connections and free
   close(sockfd);
   free(res);
   return 0;
}
