#include "server.h"
#define PERM 0644

//Global Variables [Values Set in main()]
int queue_len           = INVALID;                              //Global integer to indicate the length of the queue
int cache_len           = INVALID;                              //Global integer to indicate the length or # of entries in the cache        
int num_worker          = INVALID;                              //Global integer to indicate the number of worker threads
int num_dispatcher      = INVALID;                              //Global integer to indicate the number of dispatcher threads      
FILE *logfile;                                                  //Global file pointer for writing to log file in worker

/* ************************ Global Hints **********************************/

int cacheIndexForEviction  = 0;                            //[Cache]           --> When using cache, how will you track which cache entry to evict from array?
int workerIndex = 0;                            //[worker()]        --> How will you track which index in the request queue to remove next?
int dispatcherIndex = 0;                        //[dispatcher()]    --> How will you know where to insert the next request received into the request queue?
int curequest = 0;                               //[multiple funct]  --> How will you update and utilize the current number of requests in the request queue?


pthread_t worker_thread[MAX_THREADS];           //[multiple funct]  --> How will you track the p_thread's that you create for workers?
pthread_t dispatcher_thread[MAX_THREADS];       //[multiple funct]  --> How will you track the p_thread's that you create for dispatchers?
int threadID[MAX_THREADS];                      //[multiple funct]  --> Might be helpful to track the ID's of your threads in a global array


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;        //What kind of locks will you need to make everything thread safe? [Hint you need multiple]
pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;     //Keeps logging safe
pthread_mutex_t cache_lock = PTHREAD_MUTEX_INITIALIZER;   //Keeps caching safe

pthread_cond_t some_content = PTHREAD_COND_INITIALIZER;  //What kind of CVs will you need  (i.e. queue full, queue empty) [Hint you need multiple]
pthread_cond_t free_space = PTHREAD_COND_INITIALIZER;
request_t req_entries[MAX_QUEUE_LEN];                    //How will you track the requests globally between threads? How will you ensure this is thread safe?


cache_entry_t* cache_entries;                                  //[Cache]  --> How will you read from, add to, etc. the cache? Likely want this to be global

/**********************************************************************************/


/*
  THE CODE STRUCTURE GIVEN BELOW IS JUST A SUGGESTION. FEEL FREE TO MODIFY AS NEEDED
*/


/* ******************************** Cache Code  ***********************************/

// Function to check whether the given request is present in cache
int getCacheIndex(char *request){
  /* TODO (GET CACHE INDEX)
  *    Description:      return the index if the request is present in the cache otherwise return INVALID
  */
  for(int i = 0; i < cache_len; i++){
    if(cache_entries[i].request == NULL){     //Means the cache spot isn't in use, so it cannot be the request
      continue;
    }
    if(strcmp(request, cache_entries[i].request) == 0){ //means we found the request
      return i;
    }
  }

  return INVALID; //getting here means we searched entire cache, and no hits
}

// Function to add the request and its file content into the cache
void addIntoCache(char *mybuf, char *memory , int memory_size){
  /* TODO (ADD CACHE)
  *    Description:      It should add the request at an index according to the cache replacement policy
  *                      Make sure to allocate/free memory when adding or replacing cache entries
  */
  if(cache_entries[cacheIndexForEviction].len != 0){  //need to free the cache entry that is being stored currently
    cache_entries[cacheIndexForEviction].len = 0;     //reset all fields
    free(cache_entries[cacheIndexForEviction].content);
    free(cache_entries[cacheIndexForEviction].request);
  }

  //Now set everything with mallocs
  cache_entries[cacheIndexForEviction].len = memory_size;
  cache_entries[cacheIndexForEviction].request = (char*) malloc(strlen(mybuf)+1);
  cache_entries[cacheIndexForEviction].content = malloc(memory_size);

  //Add in the memory with the string copy function
  strcpy(cache_entries[cacheIndexForEviction].request, mybuf);
  memcpy(cache_entries[cacheIndexForEviction].content, memory, memory_size);

  //Increment by 1, but % cache_len to make the update circular for the cache to be FIFO
  cacheIndexForEviction = (cacheIndexForEviction + 1) % cache_len;
}

// Function to clear the memory allocated to the cache
void deleteCache(){
  /* TODO (CACHE)
  *    Description:      De-allocate/free the cache memory
  */
  for(int i = 0; i < cache_len; i++){
    if(cache_entries[i].len == 0) //Means uninitialized
      continue;
    free(cache_entries[i].content);
    free(cache_entries[i].request);
  }

  free(cache_entries);
}

// Function to initialize the cache
void initCache(){
  /* TODO (CACHE)
  *    Description:      Allocate and initialize an array of cache entries of length cache size
  */
 cache_entries = malloc(cache_len * sizeof(cache_entry_t)); //manual allocation

 for(int i = 0; i < cache_len; i++){
  cache_entries[i].len = 0;     //default length 0, with null pointers
  cache_entries[i].content = NULL;
  cache_entries[i].request = NULL;
 }

}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char *mybuf) {
  /* TODO (Get Content Type)
  *    Description:      Should return the content type based on the file type in the request
  *                      (See Section 5 in Project description for more details)
  *    Hint:             Need to check the end of the string passed in to check for .html, .jpg, .gif, etc.
  */

  const char ch = '.';
  char* result;
  result = strrchr(mybuf, ch);

  if(result == NULL){
    return "text/plain";
  }

  if(strcmp(".html", result) == 0){
   return "text/html"; 
  }

  if(strcmp(".jpg", result) == 0){
    return "image/jpeg";
  }

  if(strcmp(".gif", result) == 0){
    return "image/gif";
  }

  return "text/plain";
}

// Function to open and read the file from the disk into the memory. Add necessary arguments as needed
// Hint: caller must malloc the memory space
int readFromDisk(int fd, char *mybuf, void **memory) {
  //    Description: Try and open requested file, return INVALID if you cannot meaning error
  mybuf++;
  int fp;

  if((fp = open(mybuf, O_RDONLY)) == -1){ //Attempt to open
    fprintf (stderr, "ERROR: Fail to open the file.\n");
    return INVALID;
  }

  fprintf (stderr,"The requested file path is: %s\n", mybuf); //success case
  
  struct stat buf;
  if(fstat(fp, &buf)==-1){  //getting metadata
    return INVALID;
  }
  

  int filesize = buf.st_size; //set size


 *memory = malloc(filesize);  //malloc for file contents


  if(read(fp, *memory, filesize) == -1){  //Check for read failure
    return INVALID;
  }
  /* TODO 
  *    Description:      Find the size of the file you need to read, read all of the contents into a memory location and return the file size
  *    Hint:             Using fstat or fseek could be helpful here
  *                      What do we do with files after we open them?
  */


  //TODO remove this line and follow directions above
  if(close(fp) != 0) {      //Check for close failure
    perror("Close file error - unable to close file descriptor\n");
  }

  return filesize;
}

/**********************************************************************************/

// Function to receive the path)request from the client and add to the queue
void * dispatch(void *arg) {

  /********************* DO NOT REMOVE SECTION - TOP     *********************/


  /* TODO (B.I)
  *    Description:      Get the id as an input argument from arg, set it to ID
  */
  int* arg_formatted = (int*) arg;
  int ID;
  ID = *arg_formatted;
  printf("Dispatcher [%d] ready\n", ID);

  while (1) {

    /* TODO (FOR INTERMEDIATE SUBMISSION)
    *    Description:      Receive a single request and print the conents of that request
    *                      The TODO's below are for the full submission, you do not have to use a 
    *                      buffer to receive a single request 
    *    Hint:             Helpful Functions: int accept_connection(void) | int get_request(int fd, char *filename
    *                      Recommend using the request_t structure from server.h to store the request. (Refer section 15 on the project write up)
    */
    int connection_status = accept_connection();
    if(connection_status < 0){
      printf("Request is denied\n");
      return NULL;
    }



    /* TODO (B.II)
    *    Description:      Accept client connection
    *    Utility Function: int accept_connection(void) //utils.h => Line 24
    */

    //This part is held inside connection_status



    /* TODO (B.III)
    *    Description:      Get request from the client
    *    Utility Function: int get_request(int fd, char *filename); //utils.h => Line 41
    */
    char fname[BUFF_SIZE];
    int request = get_request(connection_status, fname);
    if(request != 0){
      printf("Getting the request has failed\n");
      continue;
    }
   
    fprintf(stderr, "Dispatcher Received Request: fd[%d] request[%s]\n", connection_status, fname);

    /* TODO (B.IV)
    *    Description:      Add the request into the queue
    */
    //(1) Copy the filename from get_request into allocated memory to put on request queue
    char* new_req = (char*) malloc(strlen(fname)+1);
    strcpy(new_req, fname);

    if(strlen(new_req) >= BUFF_SIZE){
      printf("Filename is too long, request cannot be processed\n");
      free(new_req);
      continue;
    }

    //(2) Request thread safe access to the request queue
     if(pthread_mutex_lock(&lock) != 0) {                            //Error checking for system calls
      perror("mutex_lock error: unable to lock request queue.\n");
    }
    
    //(3) Check for a full queue... wait for an empty one which is signaled from req_queue_notfull
    while(curequest >= MAX_QUEUE_LEN) {   //if number of requests in queue >= MAX_QUEUE_LEN
      printf("Queue has no space... %d\n", curequest);
      if(pthread_cond_wait(&free_space, &lock) != 0) {   //wait for free_space to be available, signaled by worker() method
        perror("conditional wait error - unable to wait for free space.\n");
      } 
    }

    //(4) Insert the request into the queue
    req_entries[dispatcherIndex].fd = connection_status;
    req_entries[dispatcherIndex].request = (char*) malloc(strlen(new_req)+1);
    strcpy(req_entries[dispatcherIndex].request, new_req);
    free(new_req);

    //---------------------------------------------------------

    //(5) Update the queue index in a circular fashion
    dispatcherIndex = (dispatcherIndex+1)%MAX_QUEUE_LEN;  //Update dispatcherIndex, loop back if it exceeds MAX_QUEUE_LEN
    curequest += 1;                                       //Increase number of elements in queue

    //(6) Release the lock on the request queue and signal that the queue is not empty anymore
    if(pthread_cond_signal(&some_content) != 0){           //signal that there is some content and buffer no longer empty to worker() method.
      perror("Signaling something in queue has failed\n");
    }

    if(pthread_mutex_unlock(&lock) < 0){                    //unlock request queue
      perror("Failed to release lock\n");
    }

 }

  return NULL;
}

/**********************************************************************************/
// Function to retrieve the request from the queue, process it and then return a result to the client
void * worker(void *arg) {
  /********************* DO NOT REMOVE SECTION - BOTTOM      *********************/


  //#pragma GCC diagnostic ignored "-Wunused-variable"      //TODO --> Remove these before submission and fix warnings
  //#pragma GCC diagnostic push                             //TODO --> Remove these before submission and fix warnings


  // Helpful/Suggested Declarations
  //Updated num_request to static because it must update each iteration instead of being overwritten.
  static int num_request = 0;                                    //Integer for tracking each request for printing into the log
  bool cache_hit  = false;                                //Boolean flag for tracking cache hits or misses if doing 
  int filesize    = 0;                                    //Integer for holding the file size returned from readFromDisk or the cache
  void *memory    = NULL;                                 //memory pointer where contents being requested are read and stored
  int fd          = INVALID;                              //Integer to hold the file descriptor of incoming request
  char mybuf[BUFF_SIZE];                                  //String to hold the file path from the request

  //#pragma GCC diagnostic pop                              //TODO --> Remove these before submission and fix warnings

  /* TODO (C.I)
  *    Description:      Get the id as an input argument from arg, set it to ID
  */
  int* arg_formatted = (int*) arg;
  int ID;
  ID = *arg_formatted;
  printf("Worker [%d] ready\n", ID);




  while (1) {
    /* TODO (C.II)
    *    Description:      Get the request from the queue and do as follows
    */
    
    //(1) Request thread safe access to the request queue by getting the req_queue_mutex lock
    if(pthread_mutex_lock(&lock) != 0) {
      perror("Lock error - unable to request thread safe access to request queue.\n");
    }

    //(2) While the request queue is empty conditionally wait for the request queue lock once the not empty signal is raised
    while(curequest == 0) {                     //While request queue is empty   
      if(pthread_cond_wait(&some_content, &lock) == 0) {    //wait for buffer to be non-empty
        perror("conditional wait error - unable to wait for buffer.\n");
      }
    }

    //(3) Now that you have the lock AND the queue is not empty, read from the request queue
    fd = req_entries[workerIndex].fd;
    strcpy(mybuf,req_entries[workerIndex].request);

    //Remove from the queue
    req_entries[workerIndex].fd = 0;
    free(req_entries[workerIndex].request);

    //(4) Update the request queue remove index in a circular fashion
    workerIndex = (workerIndex + 1)%MAX_QUEUE_LEN;        //Increment workerIndex, loop back if needed
    curequest -= 1;                                       //One less item in the ciruclar buffer to process


    //(5) Check for a path with only a "/" if that is the case add index.html to it
    char *test = "/index.html";
    char mybuf2[BUFF_SIZE];
    if(strcmp(mybuf, "/") == 0) {     
      strcpy(mybuf2, test);
    }
    else{
      strcpy(mybuf2, mybuf);
    }


    //(6) Fire the request queue not full signal to indicate the queue has a slot opened up and release the request queue lock
    if(pthread_cond_signal(&free_space) != 0) {
      perror("Conditional signal error - unable to signal queue not full.\n"); //signal that buffer has free space to dispatcher() method.
    }

    if(pthread_mutex_unlock(&lock) != 0) {
      perror("Thread unlock error - unable to unlock request queue.\n");
    }

    /* TODO (C.III)
    *    Description:      Get the data from the disk or the cache 
    *    Local Function:   int readFromDisk(//necessary arguments//);
    *                      int getCacheIndex(char *request);  
    *                      void addIntoCache(char *mybuf, char *memory , int memory_size);  
    */

    if(pthread_mutex_lock(&cache_lock) != 0) {      //error checking for system calls
      perror("lock error - unable to lock cache.\n");
    }
    int cache_idx = getCacheIndex(mybuf2);
    if(cache_idx != INVALID) { //Cache hit
      cache_hit = true;
      filesize = cache_entries[cache_idx].len;                //Set filesize
      memory = malloc(cache_entries[cache_idx].len);
      memcpy(memory, cache_entries[cache_idx].content, cache_entries[cache_idx].len);  //Set memory
      //Note: we don't need to set mybuf2 to request since they already are equal if we had the cache hit
    }
    else { //Cache miss
      cache_hit = false;
      filesize = readFromDisk(fd, mybuf2, &memory);

      if(filesize == INVALID){  //Reading issue
        printf("File could not be found\n");
        if(pthread_mutex_unlock(&cache_lock) != 0) {    //Unlock cache so it can handle other issues now
          perror("unlock error - unable to unlock cache.\n");
        }

        if(pthread_mutex_lock(&log_lock) != 0) {       //log that a failure was attempted
          perror("lock error - unable to lock log request.\n");
        }

        LogPrettyPrint(stdout, ID, num_request, fd, mybuf2, filesize, cache_hit); //write to terminal
        LogPrettyPrint(logfile, ID, num_request, fd, mybuf2, filesize, cache_hit);     //write into log file
        num_request += 1;             //update number of requests serviced so far

        if(pthread_mutex_unlock(&log_lock) != 0) {    //unlock the logs
          perror("unlock error - unable to unlock log request.\n");
        }
        return_error(fd, "ERROR 404\n");  //Return the error
        continue;                     //we continue here because adding into cache will cause a segmentation fault, and there's no real reason to continue
      }

      addIntoCache(mybuf2, memory, filesize);
    }

    if(pthread_mutex_unlock(&cache_lock) != 0) {
      perror("unlock error - unable to unlock cache.\n");
    }



    /* TODO (C.IV)
    *    Description:      Log the request into the file and terminal
    *    Utility Function: LogPrettyPrint(FILE* to_write, int threadId, int requestNumber, int file_descriptor, char* request_str, int num_bytes_or_error, bool cache_hit);
    *    Hint:             Call LogPrettyPrint with to_write = NULL which will print to the terminal
    *                      You will need to lock and unlock the logfile to write to it in a thread safe manor
    */
    if(pthread_mutex_lock(&log_lock) != 0) {
      perror("lock error - unable to lock log request.\n");
    }


    LogPrettyPrint(stdout, ID, num_request, fd, mybuf2, filesize, cache_hit); //write to terminal
    LogPrettyPrint(logfile, ID, num_request, fd, mybuf2, filesize, cache_hit);     //write into log file
    num_request += 1;             //update number of requests serviced so far
    

    if(pthread_mutex_unlock(&log_lock) != 0) {
      perror("unlock error - unable to unlock log request.\n");
    }

    /* TODO (C.V)
    *    Description:      Get the content type and return the result or error
    *    Utility Function: (1) int return_result(int fd, char *content_type, char *buf, int numbytes); //look in utils.h 
    *                      (2) int return_error(int fd, char *buf); //look in utils.h 
    */
    //0 on success, nonzero on failure
    char* content_type = getContentType(mybuf2);

    
    if(return_result(fd, content_type, memory, filesize) == 0) { 
      printf("Request is returned no errors.\n");
    }
    

    else{
      printf("Some type of error has occured\n");
      if(return_error(fd, "Failed Proper Return\n") != 0){
        printf("There was an error returning the problem type\n");
      }
    }

    free(memory); //All done, so we have no use for memory. What was stored in memory should be in the cache
  }
  return NULL;
}

/**********************************************************************************/

int main(int argc, char **argv) {

  /********************* Dreturn resulfO NOT REMOVE SECTION - TOP     *********************/
  // Error check on number of arguments
  if(argc != 7){
    printf("usage: %s port path num_dispatcher num_workers queue_length cache_size\n", argv[0]);
    return -1;
  }


  int port            = -1;
  char path[PATH_MAX] = "no path set\0";
  num_dispatcher      = -1;                               //global variable
  num_worker          = -1;                               //global variable
  queue_len           = -1;                               //global variable
  cache_len           = -1;                               //global variable


  /********************* DO NOT REMOVE SECTION - BOTTOM  *********************/
  /* TODO (A.I)
  *    Description:      Get the input args --> (1) port (2) path (3) num_dispatcher (4) num_workers  (5) queue_length (6) cache_size
  */

  port = atoi(argv[1]);
  strncpy(path, argv[2], PATH_MAX);
  num_dispatcher = atoi(argv[3]);
  num_worker = atoi(argv[4]);
  queue_len = atoi(argv[5]);
  cache_len = atoi(argv[6]);



  /* TODO (A.II)
  *    Description:     Perform error checks on the input arguments
  *    Hints:           (1) port: {Should be >= MIN_PORT and <= MAX_PORT} | (2) path: {Consider checking if path exists (or will be caught later)}
  *                     (3) num_dispatcher: {Should be >= 1 and <= MAX_THREADS} | (4) num_workers: {Should be >= 1 and <= MAX_THREADS}
  *                     (5) queue_length: {Should be >= 1 and <= MAX_QUEUE_LEN} | (6) cache_size: {Should be >= 1 and <= MAX_CE}
  */

  //Port check
  if(port < MIN_PORT || port > MAX_PORT){
    printf("Port number is invalid\n");
    return -1;
  }

  //Path check
  struct stat buf;
  if(stat(path, &buf) == -1){
    printf("Path is invalid\n");
    return -1;
  }

  //Dispatcher number check
  if(num_dispatcher < 1 || num_dispatcher > MAX_THREADS){
    printf("Dispatcher number is invalid\n");
    return -1;
  }

  //Worker number check
  if(num_worker < 1 || num_worker > MAX_THREADS){
    printf("Worker number is invalid\n");
    return -1;
  }

  //Queue length check
  if(queue_len < 1 || queue_len > MAX_QUEUE_LEN){
    printf("Queue length is invalid\n");
    return -1;
  }

  //Cache size check
  if(cache_len < 1 || cache_len > MAX_CE){
    printf("Cache size is invalid\n");
    return -1;
  }




  /********************* DO NOT REMOVE SECTION - TOP    *********************/
  printf("Arguments Verified:\n\
    Port:           [%d]\n\
    Path:           [%s]\n\
    num_dispatcher: [%d]\n\
    num_workers:    [%d]\n\
    queue_length:   [%d]\n\
    cache_size:     [%d]\n\n", port, path, num_dispatcher, num_worker, queue_len, cache_len);
  /********************* DO NOT REMOVE SECTION - BOTTOM  *********************/


  /* TODO (A.III)
  *    Description:      Open log file
  *    Hint:             Use Global "File* logfile", use "web_server_log" as the name, what open flags do you want?
  */
  logfile = fopen(LOG_FILE_NAME, "w");
  if(logfile == NULL){
    printf("Error opening file\n");
    return -1;
  }


  /* TODO (A.IV)
  *    Description:      Change the current working directory to server root directory
  *    Hint:             Check for error!
  */
  if(chdir(path) == -1){
    printf("Error changing current working directory to server root directory\n");
    return -1;
  }

  char arr[100];
  printf("****%s****\n", getcwd(arr, 100));




  /* TODO (A.V)
  *    Description:      Initialize cache  
  *    Local Function:   void    initCache();
  */
  initCache();




  /* TODO (A.VI)
  *    Description:      Start the server
  *    Utility Function: void init(int port); //look in utils.h 
  */
  init(port);




  /* TODO (A.VII)
  *    Description:      Create dispatcher and worker threads 
  *    Hints:            Use pthread_create, you will want to store pthread's globally
  *                      You will want to initialize some kind of global array to pass in thread ID's
  *                      How should you track this p_thread so you can terminate it later? [global]
  */
  for(int i = 0; i < MAX_THREADS; i++){
    threadID[i] = i;
  }

  for(int j = 0; j < num_worker; j++){
    if(pthread_create(&(worker_thread[j]), NULL, worker, (void *) &threadID[j]) != 0)
      printf("Thread %d  in worker_thread failed to create\n", j);
  }

  for(int k = 0; k < num_dispatcher; k++){
    if(pthread_create(&(dispatcher_thread[k]), NULL, dispatch, (void *) &threadID[k]) != 0)
      printf("Thread %d  in dispatcher_thread failed to create\n", k);
  }




  // Wait for each of the threads to complete their work
  // Threads (if created) will not exit (see while loop), but this keeps main from exiting
  int i;
  for(i = 0; i < num_worker; i++){
    fprintf(stderr, "JOINING WORKER %d \n",i);
    if((pthread_join(worker_thread[i], NULL)) != 0){
      printf("ERROR : Fail to join worker thread %d.\n", i);
    }
  }
  for(i = 0; i < num_dispatcher; i++){
    fprintf(stderr, "JOINING DISPATCHER %d \n",i);
    if((pthread_join(dispatcher_thread[i], NULL)) != 0){
      printf("ERROR : Fail to join dispatcher thread %d.\n", i);
    }
  }
  fprintf(stderr, "SERVER DONE \n");  // will never be reached in SOLUTION
}

