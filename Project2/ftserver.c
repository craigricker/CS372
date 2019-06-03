/*
 ProgramName:        ftserver.c
 Author:             Craig Ricker
 Course Name:        CS372 Intro to Computer Networks
 Last Modified:      June 2nd, 2019
 Description:
 
 This acts as a server which can send it's directory's contents, or a
 single file over a socket. It starts on a port provided by the command lien
 and listens for an incoming connection. It then reads in the information
 required to create a socket, command, and potentially a file.
 
 It then executes the command, while validating the file if required
 
 The methodology of connecting to sockets was learned/borrowed  from
 Beej's guide to networking - most specifically the setup_address
 and socket_setup functions in this file.
 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_CONNS 5
/*
 * Quick & dirty error print + quit
 * Pre condition: None
 * Inputs: code, error code to return, error message to print
 * Returns: none
 * Post Conditions: program exits immediatly
 */
void error(int code, const char *error) {
   fprintf(stderr, "%s\n", error);
   exit(code);
}

/*
 * Creates a connection to the socket
 * Pre condition: input has been validated and is working
 * Inputs:  addr, address to connect to
 *          port: port to connect to
 * Returns: new connection
 * Post Conditions:
 */
struct addrinfo *createConnection(char *addr, char *port) {
   struct addrinfo hints;
   struct addrinfo *res;
   int status = 0;
   memset(&hints, 0, sizeof hints);
   // Setup for TCP style connection
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   if ((status = getaddrinfo(addr, port, &hints, &res)) != 0) {
      error(2, "Error establishing connection.\n");
   }
   return res;
}

/*
 * Creates a connection to the socket
 * Pre condition: input has been validated and is working
 * Inputs:  addr, address to connect to
 *          port: port to connect to
 * Returns: new connection
 * Post Conditions:
 */
struct addrinfo *openConnection(char *port) {
   struct addrinfo hints;
   struct addrinfo *res;
   int status = 0;
   memset(&hints, 0, sizeof hints);
   // Setup for TCP style connection
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
      error(2, "Error establishing connection.\n");
   }
   return res;
}



/*
 * Creates a socket to conn
 * Pre condition: conn is valid
 * Inputs: conn a valid struct
 * Returns: newly created file descriptor
 * Post Conditions: file descriptor is added
 */
int createSocket(struct addrinfo *conn) {
   int sockfd;
   if ((sockfd = socket(conn->ai_family, conn->ai_socktype, conn->ai_protocol)) == -1) {
      error(-1, "Failed to create socket.\n");
   }
   
   return sockfd;
}

/*
 * Connects so socket
 * Pre condition: input valid
 * Inputs:  sockfd: file descriptor
 *          res info attached
 * Returns: None
 * Post Conditions: connection is connected
 */
void connectSocket(int sockfd, struct addrinfo * res){
   int status;
   if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1){
      error(1, "Error in connection.\n");
   }
}

/*
 * Binds socket
 * Pre condition:
 * Inputs: file descriptor and a connection
 * Returns: None
 * Post Conditions: socket is bound
 */
void bindSocket(int sockfd, struct addrinfo *conn) {
   if (bind(sockfd, conn->ai_addr, conn->ai_addrlen) == -1) {
      close(sockfd);
      error(1, "Could not bind socket.\n");
   }
}

/*
 * Listens to socket for connection
 * Pre condition: socfd is established and ready to listen
 * Inputs: working fd
 * Returns: none
 * Post Conditions: Socket waits for connection
 */
void listenSocket(int sockfd) {
   if (listen(sockfd, MAX_CONNS) == -1) {
      close(sockfd);
      error(1, "Could not listen to socket.\n");
   }
}

/*
 * Goes through the current directory and sends a list of all
 * the files to the connection
 * The below link was incredibly helpful for understanding how
 * to navigate files in C
 * https://www.sanfoundry.com/c-program-list-files-directory/
 * The files .. and . are special files, and are ignored
 * Pre condition: connection is active
 * Inputs: connection - where to send the file names
 * Returns: none
 * Post Conditions: file names are sent
 */
void list_files(int connection) {
// https://www.sanfoundry.com/c-program-list-files-directory/
   DIR * d;
   struct dirent * dir;
   char buffer[100] = "";
   d = opendir(".");
   if (d)
   {
      // Look at every single file
      while ((dir = readdir(d)) != NULL) {
         memset(&buffer, 0, sizeof buffer);
         strcpy(buffer, dir->d_name);
         // Don't care about . or ..
         if ((strcmp( buffer, ".") != 0) && (strcmp(buffer, "..") !=0)) {
            strcat(buffer, " ");
            send(connection, buffer, strlen(buffer), 0);
         }
      }
      closedir(d);
   }
}

/*
 * Reads through file 2000 chars at a time, and sends the
 * buffer repeatedly to connection until it is fully sent
 * reads more of the file, and then sends that
 * Control loop adopted from my CS344 assignment/instructors lectures
 * Pre condition: fname has been validated as a readable file
 * Inputs: datafd: where to send the data
 *          fname: file to read
 * Returns: None
 * Post Conditions: file is transfered to datafd
 */
void send_file(int datafd, char *fname) {
   char buffer[2000] = "";
   int reader = open(fname, O_RDONLY);
   int num_read, num_written;
   char * scroller;
   // Guarantees that you read to the end of the file
   while (1) {
      num_read = read(reader, buffer, sizeof(buffer) - 1);
      // You've read to the end of the file
      if (num_read == 0)
         break;
      // Something has gone WRONG
      else if (num_read < 0)
         error(1, "Failed to read file\n");
      // Send everything that you have read so far
      else {
         scroller = buffer;
         // You know how much you send, subtract from what is left
         while (num_read > 0)
         {
            // Store number send
            num_written = send(datafd, scroller, sizeof(buffer),0);
            if (num_written < 0)
               error(1, "Failed to write\n");
            // Calculate what is left to send
            num_read -= num_written;
            // Update what you send next
            scroller += num_written;
         }
         memset(buffer, 0, sizeof(buffer));
      }
   }
}

/*
 * Using connection, it receives a port, an address, and a command
 * it then executes the command given . It can either send a
 * file over the socket that is described by the received port/address
 * or it can send the directory contents
 * Pre condition: connection is established
 * Inputs:  connection: where to send data
 * Returns: None
 * Post Conditions:  command is executed
 */
void handle_connection(int connection) {
   char port[100] = "";
   char host[100] = "";
   char command[100] = "";
   char fname[100] = "";
   char * valid = "valid";
   char * invalid = "invalid";
   struct addrinfo* res;
   int datafd = 0;

   // Get the port
   recv(connection, port, sizeof(port) - 1, 0);
   send(connection, valid, sizeof(valid), 0);
   // Get host
   recv(connection, host, sizeof(host) - 1, 0);
   send(connection, valid, sizeof(valid), 0);
   printf("Connection from %s.\n", host);
   // Get command
   recv(connection, command, sizeof(command) - 1, 0);
   
   // Ensure valid command is entered, and alert client
   if ((strcmp(command, "-g") == 0) || (strcmp(command, "-l") == 0)) {
      send(connection, valid, sizeof(valid), 0);
   }
   else {
      printf("Invalid command!\n");
      send(connection, invalid, sizeof(invalid), 0);
   }
   // Create connection to send information back
   if (strcmp(command, "-l") == 0)
   {
      sleep(1);
      printf("List directory request on port %s.\n", port);
      printf("Sending directory contents to %s:%s\n", host, port);
      res = createConnection(host, port);
      datafd = createSocket(res);
      connectSocket(datafd, res);
      list_files(datafd);
   }
   else
   {
      // https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
      // Used above link to detect if file exists or not
      recv(connection, fname, sizeof(fname), 0);
      printf("File \"%s\" requested on port %s\n", fname, port);
      if (access( fname, F_OK ) != -1) {
         send(connection, valid, sizeof(valid), 0);
         printf("Sending \"%s\" on port %s\n", fname, port);
      }
      else {
         printf("File not found. Sending error message.\n");
         send(connection, invalid, sizeof(invalid), 0);
         close(datafd);
         return;
      }
      sleep(1);
      res = createConnection(host, port);
      datafd = createSocket(res);
      connectSocket(datafd, res);
      send_file(datafd, fname);
   }
   close(datafd);
}

/*
 * Sits and waits for incoming connection, once connection is received
 * hands off connection to other function
 * Pre condition: sockfd been connected to
 * Inputs:  sockfd connection to listen on
 * Returns: None
 * Post Conditions: Command is executed
 */
void ftclient(int sockfd) {
   struct sockaddr_storage clientAddr;
   socklen_t addrSize;
   int newConnection;
   
   while (1) {
      addrSize = sizeof(clientAddr);
      printf("Waiting for connection...\n");
      newConnection = accept(sockfd, (struct sockaddr *)&clientAddr, &addrSize);
      handle_connection(newConnection);
      close(newConnection);
   }
}

int main(int argc, char *argv[]) {
   if (argc != 2)
      error(1, "Usage: ./ftpserver [PORTNUBER}\n");
   int port_n = atoi(argv[1]);
   if ((port_n >65535) || (port_n < 10000))
      error(1, "Enter a valid port number\n");
   printf("Server open on %s\n", argv[1]);
   struct addrinfo* res = openConnection(argv[1]);
   int sockfd = createSocket(res);
   bindSocket(sockfd, res);
   listenSocket(sockfd);
   ftclient(sockfd);
   freeaddrinfo(res);
}
