/*
 Craig Ricker
 rickercr
 March 17th, 2019
 This is the client for decryption
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

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

void clear_buffer() {
   int c;
   while ((c = getchar()) != '\n' && c != EOF) { }
}
bool send_message(int sockfd, char * username) {
//   printf("In send message\n");
   char message[DATASIZE];
   char input[DATASIZE - 12];
   int bytes;
   memset((char*)&message, '\0', sizeof(message));
   memset((char*)&input, '\0', sizeof(input));
   strcat(message, username);
   strcat(message, PROMPT);
   printf("%s", message);
//   clear_buffer();
//   printf("Successfully cleared buffer\n");
   fgets(input, 500, stdin);
//   scanf("%s", input);
//   printf("Got line\n");
   strcat(message, input);
//   printf("Message to send:\n%s\n", message);
   bytes = send(sockfd, message, strlen(message), 0);
   
   if (bytes == -1) {
      fprintf(stderr, "Failure to send data\n");
      exit(1);
   }
   
   if (strstr(message, QUIT_PROMPT) != NULL)
      return false;
   else
      return true;
}

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


void chat(int sockfd, char * username) {
//   printf("In chat\n");
//   while ((send_message(sockfd, username)
//           && receive_mesage(sockfd))) {
//
//
//   }
   while ((send_message(sockfd, username) &&
           receive_mesage(sockfd))) {
      
      
   }
}

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

int socket_setup(struct addrinfo * res) {
   int sockfd, status;
   
   if ((sockfd = socket(res->ai_family, res->ai_socktype,
                        res->ai_protocol)) == -1){
      fprintf(stderr, "Error. Socket creation failure.\n");
      exit(1);
   }
   
   if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1){
      fprintf(stderr, "Error connecting to socket.\n");
      exit(1);
   }
   
   return sockfd;
}


int main(int argc, char *argv[])
{
   char userName[USERSIZE];
   int sockfd;
   struct addrinfo * res;
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
   sockfd = socket_setup(res);
   chat(sockfd, userName);
//   send_message(0, userName);
   
   
   
//   int socketFD, portNumber, charsWritten, charsRead;
//   struct sockaddr_in serverAddress;
//   struct hostent* serverHostInfo;
//   char * message = NULL,
//   * DEC_AUTH = "DEC!",
//   * key = NULL,
//   * end = NULL,
//   * to_send = NULL;
//
//   // TODO: Add check to ensure port number is all digits
//   if (argc != 4 || !all_digits(argv[3])) {
//      fprintf(stderr,"USAGE: %s file name, keyfile, port\n", argv[0]);
//      exit(0);
//   } // Check usage & args
//
//
//   FILE * reader;    // Use same variable for both files?
//   size_t file_size = 0, key_size;
//   key_size = 0;
//
//
////   printf("You are in otp_enc\n");
//
////   printf("You are about to read message\n");
//   // Read the contents of file to be encryped into buffer
//   reader = fopen(argv[1], "r");
//   if (!reader) {             // Check file open
//      fprintf(stderr, "Failed to open file %s\n", argv[1]);
//      return 1;
//   }
////   printf("You opened the file\n");
//
//   file_size = getline(&message, &file_size, reader);
//   fclose(reader);
//   if (file_size < 0) {       // Check read was successful
//      fprintf(stderr, "Error reading plaintext from %s\n", argv[1]);
//      free(message);
//      return 1;
//   }
////   printf("Message is %s\n", message);
//   // No illegal characters allowed
//   if (!all_valid_chars(message)) {
//      free(message);
//      fprintf(stderr, "Illegal characters in %s\n", argv[1]);
//      return 1;
//   }
//   // Add terminater character to end of message
//   end = strchr(message, '\n');
//   *end = MESSAGE_DELIM;
////   printf("Finished reading message\n%s", message);
//
//
//   // Read the contents of keyfile
//   reader = fopen(argv[2], "r");
//   if (!reader) {             // Check file open
//      fprintf(stderr, "Failed to open file %s\n", argv[2]);
//      return 1;
//   }
//   key_size = getline(&key, &key_size, reader);
//   fclose(reader);
//   if (key_size < 0) {       // Check read was successful
//      fprintf(stderr, "Error reading keyfile from %s\n", argv[2]);
//      free(message);
//      return 1;
//   }
//   else if (!all_valid_chars(key)) {
//      free(key);
//      free(message);
//      fprintf(stderr, "Illegal characters in %s\n", argv[2]);
//      return 1;
//   }
////   printf("Finishred reading keyfile\n");
//   // Add termin to end of key
//   key[strlen(message) - 1] = KEY_DELIM;
//
////   printf("Key is %sNoNewLine\n", key);
//   // Ensure that key is large enough for file
//   if (strlen(key) < strlen(message)) {
//      fprintf(stderr, "Key was too short in %s\n", argv[2]);
//      free(message);
//      free(key);
//      return 1;
//   }
////   printf("Done reading key:\n%s\n%s\n", key, message);
//
//
//   // Create space for combined strings
//
//   to_send =  calloc(strlen(message) * 2, sizeof(char));
//   strcat(to_send, message);
////   printf("Before strncat to send is\n%s\n", to_send);
//   // Don't need entire key
//   strncat(to_send, key, strlen(message));
////   printf("After strncat to send is\n%s\n", to_send);
//   free(key);
//   key = NULL;
//   free(message);
//   message = NULL;
//
//
//
////   printf("To send\n%s\n", to_send);
//
//
////    Set up the server address struct
//   memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
////   printf("Clearned server addres\n");
//   portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
////   printf("Set port number to %s\n", argv[3]);
//   serverAddress.sin_family = AF_INET; // Create a network-capable socket
//   serverAddress.sin_port = htons(portNumber); // Store the port number
//   serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
//   if (serverHostInfo == NULL) {
//      fprintf(stderr, "CLIENT: ERROR, no such host on port %d\n",
//              portNumber);
//      exit(2);
//   }
////   printf("Second membset of serverAdress\n");
//   memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
//
//   // Set up the socket
////   printf("Set up socket!\n");
//   socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
//   if (socketFD < 0) {
//      fprintf(stderr, "CLIENT: ERROR opening socket on port %d\n", portNumber);
//      exit(2);
//   }
//
//
//   // Connect to server
////   printf("Connect to server!\n");
//   if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
//      fprintf(stderr, "CLIENT: ERROR connecting using port %d\n",
//              portNumber);
//      exit(2);
//   }
//
////   printf("Connected to server!");
//
//   // Send message to server
////      printf("About to send first handshake!\n");
//   charsWritten = sendMessage(socketFD, DEC_AUTH);
////   printf("Sent first handshake!\n");
//   if (charsWritten < 0) {
//      fprintf(stderr, "CLIENT: ERROR writing to socket using port %d\n",
//              portNumber);
//      exit(2);
//   }
//   if (charsWritten < strlen(DEC_AUTH)) {
//      fprintf(stderr, "CLIENT: WARNING: Not all data written to socket! Port %d\n", portNumber);
//      exit(2);
//   }
//
//   // See if the handshake back is correct
//   charsRead = receiveMessage(socketFD,
//                              &message, '!');
//   if (!strstr(message, DEC_AUTH) || charsRead < 0) {
//      fprintf(stderr, "%s\n", message);
//      exit(2);
//   }
//   else {
//      sendMessage(socketFD, to_send);
//      free(message);
//      message = NULL;
//   }
//
//   // Read encrypted message
//   charsRead = receiveMessage(socketFD,
//                              &message, '#');
//   message[strlen(message) - 1] = 0;
//   printf("%s\n", message);
//   free(message);
//   message = NULL;
//   // Close the socket
//   close(socketFD);
   return 0;
}
