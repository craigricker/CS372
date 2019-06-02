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
 
 The methodology of connecting to sockets was learned/borrowed  from
 Beej's guide to networking - most specifically the setup_address
 and socket_setup functions in this file.
 
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
#define MAX_CONNS 1

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
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   
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
int socket_setup(struct addrinfo * res) {
   int sockfd, status, bytes;
   
   // Attempt socket creation
   if ((sockfd = socket(res->ai_family, res->ai_socktype,
                        res->ai_protocol)) == -1){
      fprintf(stderr, "Error. Socket creation failure.\n");
      exit(1);
   }
   
   return sockfd;
}

void listenSocket(int sockfd) {
   if (listen(sockfd, MAX_CONNS) == -1) {
      close(sockfd);
      perror("Error listening to socket.\n");
      exit(1);
   }

}

void wait_connection(int sockfd) {
   struct sockaddr_storage in_add;
   socklen_t add_size;
   int in_connect;
   
   while (1) {
      add_size = sizeof(in_add);
      in_connect = accept(sockfd, (struct sockaddr*) & in_add, &add_size);
      
      // Check for successful connection
      if (in_connect == -1)
         continue;
      
      // Actually talk to client
      return;
   }
   
}

int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("Usage: ./ftserver [PORT]\n");
      exit(1);
   }
   printf("Server open on %s\n", argv[1]);
   // Setup the address
   struct addrinfo * res = setup_addres(NULL, argv[1]);
   int sockfd = socket_setup(res);
   listenSocket(sockfd);
   wait_connection(sockfd);
   
}
