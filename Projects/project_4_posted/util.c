#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int master_fd = -1;
pthread_mutex_t accept_con_mutex = PTHREAD_MUTEX_INITIALIZER;

/**********************************************
 * init
   - port is the number of the port you want the server to be
     started on
   - initializes the connection acception/handling system
   - if init encounters any errors, it will call exit().
************************************************/
void init(int port) {
   int sd;
   struct sockaddr_in addr;
   int ret_val;
   //int flag;
   
   /**********************************************
    * IMPORTANT!
    * ALL TODOS FOR THIS FUNCTION MUST BE COMPLETED FOR THE INTERIM SUBMISSION!!!!
    **********************************************/
   
   // TODO: Create a socket and save the file descriptor to sd (declared above)
   // This socket should be for use with IPv4 and for a TCP connection.
   sd = socket(AF_INET, SOCK_STREAM, 0);      //Socket descriptor saved to sd
   if(sd < 0) {                           //Error checking for system calls.
    printf("Socket error - unable to create socket.\n");
    ret_val = 1;
    exit(ret_val);
   }

   // TODO: Change the socket options to be reusable using setsockopt(). 
   int enable = 1;                                                          //Enable reusable socket options
   if(setsockopt(sd, SOL_SOCKET,SO_REUSEADDR, (char*) &enable, sizeof(int)) < 0){  //Set to avoid collisions before binding
    perror("unable to set socket options to avoid collisions\n");
    ret_val = 1;
    exit(ret_val);
   }

   // TODO: Bind the socket to the provided port.
   addr.sin_family = AF_INET;             
   addr.sin_addr.s_addr = htonl(INADDR_ANY);      //OS picks IP address
   addr.sin_port = htons(port);                   //Network byte order
   bind(sd, (struct sockaddr*) &addr, sizeof(addr)); //Bind socket to provided port  

   // TODO: Mark the socket as a pasive socket. (ie: a socket that will be used to receive connections)
   int backlog = 5;
   int listen_status = listen(sd, backlog);          //setup queue of size 5 when receiving requests.
   if(listen_status < 0) {                       //error-checking for system calls
    printf("Listen error - unable to listen for data.\n");
    ret_val = 1;
    exit(ret_val);
   }                                                 //sockaddr is the addr for the client socket
   // We save the file descriptor to a global variable so that we can use it in accept_connection().
   master_fd = sd;
   printf("UTILS.O: Server Started on Port %d\n",port);
}





/**********************************************
 * accept_connection - takes no parameters
   - returns a file descriptor for further request processing.
     DO NOT use the file descriptor on your own -- use
     get_request() instead.
   - if the return value is negative, the thread calling
     accept_connection must should ignore request.
***********************************************/
int accept_connection(void) {
   int newsock = master_fd;
   struct sockaddr_in new_recv_addr;
   uint addr_len;
   addr_len = sizeof(new_recv_addr);
   
   /**********************************************
    * IMPORTANT!
    * ALL TODOS FOR THIS FUNCTION MUST BE COMPLETED FOR THE INTERIM SUBMISSION!!!!
    **********************************************/
   // TODO: Aquire the mutex lock
   if(pthread_mutex_lock(&accept_con_mutex) != 0) {      //acquire lock and enable error checking
    printf("Unable to acquire mutex lock for connection.\n");
   }
   // TODO: Accept a new connection on the passive socket and save the fd to newsock
   newsock = accept(newsock, (struct sockaddr*) &new_recv_addr, &addr_len);  //blocks until request arrives - stores fd to socket in request_fd
   if(newsock < 0) {                         //error-checking for system calls
    printf("Request error - unable to obtain receiving socket.\n");
   }
   // TODO: Release the mutex lock
   if (pthread_mutex_unlock(&accept_con_mutex) != 0) {   //release lock and enable error checking.
    printf("Unable to unlock mutext after connection.\n");
   }
   // TODO: Return the file descriptor for the new client connection
   return newsock;              //README - there may still be more to do here.
}





/**********************************************
 * get_request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        from where you wish to get a request
      - filename is the location of a character buffer in which
        this function should store the requested filename. (Buffer
        should be of size 1024 bytes.)
   - returns 0 on success, nonzero on failure. You must account
     for failures because some connections might send faulty
     requests. This is a recoverable error - you must not exit
     inside the thread that called get_request. After an error, you
     must NOT use a return_request or return_error function for that
     specific 'connection'.
************************************************/
int get_request(int fd, char *filename) {   //assuming pass-by-reference

  /**********************************************
  * IMPORTANT!
  * THIS FUNCTION DOES NOT NEED TO BE COMPLETE FOR THE INTERIM SUBMISSION, BUT YOU WILL NEED
  * CODE IN IT FOR THE INTERIM SUBMISSION!!!!! 
  **********************************************/
   
  // INTERIM TODO: Read the request from the file descriptor into the buffer
  // HINT: Attempt to read 2048 bytes from the file descriptor. 
  int bufsize = 2048;   //buffer size
  char buf[bufsize];    //buffer to store reads
  if(read(fd, buf, bufsize) == -1){        //read from the file with error checking
    printf("Error reading from the file.");
  }
  
  // INTERIM TODO: Print the first line of the request to the terminal.
  //printf("First line of the request:%s ", buf);       //README - more might need to be done here.

  // TODO: Ensure that the incoming request is a properly formatted HTTP "GET" request
  // The first line of the request must be of the form: GET <file name> HTTP/1.0 
  // or: GET <file name> HTTP/1.1
   
  // HINT: It is recommended that you look up C string functions such as sscanf and strtok for
  // help with parsing the request.
  const char space[2] = " ";
  char* is_get = strtok(buf, space); //Grab part 1 of request
  char* is_fname = strtok(NULL, space); //grab part 2 of request
  char* is_http = strtok(NULL, space); //grab part 3 of request
  if(strlen(is_http) > 7){
    is_http[8] = '\0';
  }

  printf("First line of Request: ");
  printf("%s ", is_get);
  printf("%s ", is_fname);
  printf("%s\n", is_http);


  if(is_get == NULL || is_fname == NULL || is_http == NULL){
    printf("Improper format3\n");
    return -1;
  }

  if(strcmp(is_get, "GET") != 0){ //isn't a get request
    printf("Improper format1\n");
    return -1;
   }

  if(strcmp(is_http, "HTTP/1.0") != 0){ //so not 1.0 format
    if(strcmp(is_http, "HTTP/1.1") != 0){ //so not 1.1 of format
      printf("Improper format2\n");
      return -1;
    }
  }

  // TODO: Extract the file name from the request
  //Done already in is_fname

  // TODO: Ensure the file name does not contain with ".." or "//"
  // FILE NAMES WHICH CONTAIN ".." OR "//" ARE A SECURITY THREAT AND MUST NOT BE ACCEPTED!!!
  char* problem1 = strstr(is_fname, "..");
  char* problem2 = strstr(is_fname, "//");

  if(problem1 != NULL){
    printf("Security alert, cannot get request\n");
    return -1;
  }

  if(problem2 != NULL){
    printf("Security alert, cannot get request\n");
    return -1;
  }
   
   // HINT: It is recommended that you look up the strstr function for help looking for faulty file names.
   // TODO: Copy the file name to the provided buffer
  if(strlen(is_fname) <= 1024) {   //Check to make sure is_fname is less than 1024 bytes (aka 1024 chars) before copying
    strcpy(filename, is_fname);
  }
  else {
    printf("Requested filename is greater than 1024 bytes.\n");
    return -1;
  }

   return 0;
}

/**********************************************
 * return_result
   - returns the contents of a file to the requesting client
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the result of a request
      - content_type is a pointer to a string that indicates the
        type of content being returned. possible types include
        "text/html", "text/plain", "image/gif", "image/jpeg" cor-
        responding to .html, .txt, .gif, .jpg files.
      - buf is a pointer to a memory location where the requested
        file has been read into memory (the heap). return_result
        will use this memory location to return the result to the
        user. (remember to use -D_REENTRANT for CFLAGS.) you may
        safely deallocate the memory after the call to
        return_result (if it will not be cached).
      - numbytes is the number of bytes the file takes up in buf
   - returns 0 on success, nonzero on failure.
************************************************/
int return_result(int fd, char *content_type, char *buf, int numbytes) {

  // TODO: Prepare the headers for the response you will send to the client.
  // REQUIRED: The first line must be "HTTP/1.0 200 OK"
  char header1[2048];
  sprintf(header1, "HTTP/1.0 200 OK\nContent-Length: %d\nContent-Type: %s\nConnection: Close\n\n", numbytes, content_type);
  // REQUIRED: Must send a line with the header "Content-Length: <file length>"
  // REQUIRED: Must send a line with the header "Content-Type: <content type>"
  // REQUIRED: Must send a line with the header "Connection: Close"
   
  // NOTE: The items above in angle-brackes <> are placeholders. The file length should be a number
  // and the content type is a string which is passed to the function.
  printf("%s", header1);
  /* EXAMPLE HTTP RESPONSE
  * 
  * HTTP/1.0 200 OK
  * Content-Length: <content length>
  * Content-Type: <content type>
  * Connection: Close
  * 
  * <File contents>
  */
    
  // TODO: Send the HTTP headers to the client
  if(write(fd, header1, strlen(header1)) < 0){
    printf("failed to write header\n");
    return -1;
  }

  if(write(fd, buf, numbytes) < 0){
    printf("Failed to write content\n");
    return -1;
  }
    
  // IMPORTANT: Add an extra new-line to the end. There must be an empty line between the 
  // headers and the file contents, as in the example above.
    
  // TODO: Send the file contents to the client

  //Took care of this above
    
  // TODO: Close the connection to the client
  if(close(fd) < 0){
    printf("Failed to close the connnection\n");
    return -1;
  }
    
  return 0;
}





/**********************************************
 * return_error
   - returns an error message in response to a bad request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the error
      - buf is a pointer to the location of the error text
   - returns 0 on success, nonzero on failure.
************************************************/
int return_error(int fd, char *buf) {

  // TODO: Prepare the headers to send to the client
  // REQUIRED: First line must be "HTTP/1.0 404 Not Found"
  // REQUIRED: Must send a header with the line: "Content-Length: <content length>"
  // REQUIRED: Must send a header with the line: "Connection: Close"
  char header1[2048];
  sprintf(header1, "HTTP/1.0 404 Not Found\nContent-Length: %ld\nConnection: Close\n\n", strlen(buf));
   
  // NOTE: In this case, the content is what is passed to you in the argument "buf". This represents
  // a server generated error message for the user. The length of that message should be the content-length.
   
  // IMPORTANT: Similar to sending a file, there must be a blank line between the headers and the content.
  printf("%s", header1); 
   
   
  /* EXAMPLE HTTP ERROR RESPONSE
  * 
  * HTTP/1.0 404 Not Found
  * Content-Length: <content length>
  * Connection: Close
  * 
  * <Error Message>
  */
    
  // TODO: Send headers to the client
  if(write(fd, header1, strlen(header1)) < 0){
    printf("failed to write header\n");
    return -1;
  }
    
  // TODO: Send the error message to the client
  if(write(fd, buf, strlen(buf)) < 0){
    printf("Failed to write content\n");
    return -1;
  }
    
  // TODO: Close the connection with the client.
  if(close(fd) < 0){
    printf("Failed to close the connnection\n");
    return -1;
  }
    
  return 0;
}