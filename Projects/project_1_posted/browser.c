/* CSCI-4061 Fall 2022
 * Group Member #1: <Amit Manicka> <manic022>
 * Group Member #2: <Aditya Prabhu> <prabh079>
 * Group Member #3: <Ian Alexander> <alexa818>
 * 
 * Contributions:
 * Amit wrote on_blacklist's logic flow, bad_format, termination, fixed the tab creation process, and finished main
 * Aditya fixed on_blacklist's helper functions, init_blacklist, and wrote the groundwork for tab creation process and main
 * Ian unfortunately did not contribute very much
 */

#include "wrapper.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <gtk/gtk.h>

/* === PROVIDED CODE === */
// Function Definitions
void new_tab_created_cb(GtkButton *button, gpointer data);
int run_control();
int on_blacklist(char *uri);
int bad_format (char *uri);
void uri_entered_cb(GtkWidget* entry, gpointer data);
void init_blacklist (char *fname);

/* === PROVIDED CODE === */
// Global Definitions
#define MAX_TAB 100                 //Maximum number of tabs allowed
#define MAX_BAD 1000                //Maximum number of URL's in blacklist allowed
#define MAX_URL 100                 //Maximum char length of a url allowed

/* === STUDENTS IMPLEMENT=== */
// HINT: What globals might you want to declare?
char blacklisted_urls [MAX_BAD][MAX_URL];         //List of blacklisted urls
pid_t child_controller;                            //Pid of controller
int stat_loc_controller;                            //Status of controller (i.e. when to kill, wait, etc.)
int tab_count = -1;                                //Count of the number of tabs, start at -1 for indexing purposes.
pid_t tabs_list [MAX_TAB];                         //List of pids where we store our tabs                      
char *www_input_str = NULL;				   //inputCopy to be used on helper functions
char *https_input_str = NULL;        //input strings for helper functions
char *http_input_str = NULL;          //input strings for helper functions


/* === PROVIDED CODE === */
/*
 * Name:		          new_tab_created_cb
 *
 * Input arguments:	
 *      'button'      - whose click generated this callback
 *			'data'        - auxillary data passed along for handling
 *			                this event.
 *
 * Output arguments:   void
 * 
 * Description:        This is the callback function for the 'create_new_tab'
 *			               event which is generated when the user clicks the '+'
 *			               button in the controller-tab. The controller-tab
 *			               redirects the request to the parent (/router) process
 *			               which then creates a new child process for creating
 *			               and managing this new tab.
 */
// NO-OP for now
void new_tab_created_cb(GtkButton *button, gpointer data)
{
}
 
/* === PROVIDED CODE === */
/*
 * Name:                run_control
 * Output arguments:    void
 * Function:            This function will make a CONTROLLER window and be blocked until the program terminates.
 */
int run_control()
{
  // (a) Init a browser_window object
	browser_window * b_window = NULL;

	// (b) Create controller window with callback function
	create_browser(CONTROLLER_TAB, 0, G_CALLBACK(new_tab_created_cb), G_CALLBACK(uri_entered_cb), &b_window);

	// (c) enter the GTK infinite loop
	show_browser();
	return 0;
}

/* === STUDENTS IMPLEMENT=== */
/* 
    Function: on_blacklist  --> "Check if the provided URI is in th blacklist"
    Input:    char* uri     --> "URI to check against the blacklist"
    Returns:  True  (1) if uri in blacklist
              False (0) if uri not in blacklist
    Hints:
            (a) File I/O
            (b) Handle case with and without "www." (see writeup for details)
            (c) How should you handle "http://" and "https://" ??
*/ 

//Helper Functions, unlikely they cause problems, but maybe causing the segfault
//----------------------------------------------------------------------------

char* remove_www (const char *input) { //removes www. from a string
  free(www_input_str);				     //Free any previously allocated memory
  www_input_str = (char *) malloc(strlen(input)+1); //allocate + 1 for null character
  strncpy(www_input_str,input,strlen(input)+1);	   //Copy over contents to this string
  char* output = www_input_str+4;		   //Return pointer that is 4 characters forward
  return output;
} //fixed segfaulting

char* remove_https (const char *input) { //removes https:// from a string
  free(https_input_str); //Free the previous allocated memory
  https_input_str = (char *) malloc(strlen(input)+1); //allocate + 1 for null character
  strncpy(https_input_str,input,strlen(input)+1); //Copy the contents
  char* output = https_input_str+8; //Return pointer that is 8 characters forward
  return output;
} //fixed segfaulting

char* remove_http (const char *input) { //removes http:// from a string
  free(http_input_str); //Free the previous allocated memory
  http_input_str = (char *) malloc(strlen(input)+1); //allocated +1 for null character
  strncpy(http_input_str,input, strlen(input)+1); //Copy contents
  char* output = http_input_str+7; //Return pointer that is 7 characters forward
  return output;
} //fixed segfaulting

//-----------------------------------------------------------------------------

int on_blacklist (char *uri) {
  //STUDENTS IMPLEMENT

  //Need to check for www and without
  //From Talking to Jon, there is no http:// or https:// that will appear in any blacklist entry
  //Regarding uris, if this function is running, then uri will have either https:// or http://
  //This is because it must have passed bad_format to get here
  //This loop will run through the entire blacklist

  //Generate both uri versions since one will match
  char* uri_version_no_http = remove_http(uri);
  char* uri_version_no_https = remove_https(uri);
  for(int j = 0; j < MAX_BAD; j++) {
    //Grab blacklisted url
    char* black = blacklisted_urls[j];	
    
    if(strlen(black) < 5) { //by definition, too small to match any uri (minimum is x.com)
      return 0; //This means, we've gotten past the entire list
    }

    // Test prints
    //printf("uri_version_no_http: %s\n", uri_version_no_http);
    //printf("uri_version_no_https: %s\n", uri_version_no_https);

    //Check for the www
    char* www_check = "www";
    char test_www[4]; //Generate test string
    strncpy(test_www, &black[0], 3); //Copy contents over
    test_www[3] = '\0'; //Ensure the null character is added
    int result_blacklist_www = strcmp(test_www, www_check); //Compare

    char* black_without_www = black; //Define formatted blacklist url
    if(result_blacklist_www == 0){ //blacklist entry had www
      //printf("Blacklist entry have www\n");
      black_without_www = remove_www(black); //Remove the www
    }

    //Check if the input string was http://, we only test one since it either has http:// or https://
    char* http_check = "http://";
    char test_http[8]; //Define test string
    memcpy(test_http, &uri[0], 7 ); //Copy the memory over
    test_http[7] = '\0'; //Ensure null char is in

    if(strcmp(test_http, http_check) == 0){ //The uri is http://
      if(strcmp(black_without_www, uri_version_no_http) == 0){ //On blacklist
        return 1;
      }
    }
    else{ //The uri is https://
      if(strcmp(black_without_www, uri_version_no_https) == 0){ //On blacklist
        return 1;
      }
    }

  }
  return 0; //Just in case it makes it to the end and doesn't return in loop correctly
} //on_blacklist

/* === STUDENTS IMPLEMENT=== */
/* 
    Function: bad_format    --> "Check for a badly formatted url string"
    Input:    char* uri     --> "URI to check if it is bad"
    Returns:  True  (1) if uri is badly formatted 
              False (0) if uri is well formatted
    Hints:
              (a) String checking for http:// or https://
*/
int bad_format (char *uri) {
  //STUDENTS IMPLEMENT
  if(strlen(uri) < 7){ //by definition, this means neither http:// nor https:// will be in the string
    return 1;
  }
  //Define checks
  char* check1 = "http://";
  char* check2 = "https://";
  //Create test strings for http and https:// respectively
  char test1[8];
  char test2[9];

  //Copy the format of http or https into the test strings
  memcpy(test1, &uri[0], 7 );
  test1[7] = '\0';
  memcpy(test2, &uri[0], 8 );
  test2[8] = '\0';

  //Compare strings
  int result1 = strcmp(test1, check1);
  int result2 = strcmp(test2, check2);

  if(result1 != 0) { //doesnt match http://
    if(result2 != 0){ //doesn't match https://
      return 1;
    }
    return 0; //Matches https://
  }
  else{ //Matches http://
    return 0;
  }
  return 0; //just in case something goes wrong
} //This one works as of 10/3/2022

/* === STUDENTS IMPLEMENT=== */
/*
 * Name:                uri_entered_cb
 *
 * Input arguments:     
 *                      'entry'-address bar where the url was entered
 *			                'data'-auxiliary data sent along with the event
 *
 * Output arguments:     void
 * 
 * Function:             When the user hits the enter after entering the url
 *			                 in the address bar, 'activate' event is generated
 *			                 for the Widget Entry, for which 'uri_entered_cb'
 *			                 callback is called. Controller-tab captures this event
 *			                 and sends the browsing request to the router(/parent)
 *			                 process.
 * Hints:
 *                       (a) What happens if data is empty? No Url passed in? Handle that
 *                       (b) Get the URL from the GtkWidget (hint: look at wrapper.h)
 *                       (c) Print the URL you got, this is the intermediate submission
 *                       (d) Check for a bad url format THEN check if it is in the blacklist
 *                       (e) Check for number of tabs! Look at constraints section in lab
 *                       (f) Open the URL, this will need some 'forking' some 'execing' etc. 
 */
void uri_entered_cb(GtkWidget* entry, gpointer data)
{
  
  //STUDENTS IMPLEMENT
  char* output = get_entered_uri(entry);
  if(strlen(output) == 0){ //Check for null, which is a type of bad format
    printf("Url Entered is: %s \n", output);
    alert("BAD FORMAT");
    return;
  }

  int format_value = bad_format(output); //Run bad_format

  if(format_value == 1){ //Failure case of bad format
    printf("Url Entered is: %s \n", output);
    alert("BAD FORMAT");
    return;
  }
  
  //Success case for intermediate submission
  //printf("Url Entered is: %s \n", output);
  
  //Check for Blacklist
  if(on_blacklist(output) == 0) {         //If the URL entered is NOT on the blacklist
    tab_count++; //Need to increment the tab counter
    printf("URL entered is: %s\n", output); //Print out url as per the solution code
    if(tab_count > MAX_TAB-1) { //Remember, tab count starts at zero
      printf("Cannot open more URLs! Max Tab limit reached.");
      alert("TAB LIMIT REACHED");
      return;
    } 
    else { //make the tab
      tabs_list[tab_count] = fork();
      if(tabs_list[tab_count] == 0) {                    //create the tab
        char formatted_tab_string[3]; //Need to format the inputs into render
        if(tab_count < 10){
          formatted_tab_string[strlen(formatted_tab_string)-1] = '\0'; //Reduce size by 1 so there's no segfault or weird behavior
          sprintf(formatted_tab_string, "%d", tab_count);
        }
        else if(tab_count >= 10){
          sprintf(formatted_tab_string, "%d", tab_count);
        }
        execl("./render","render", formatted_tab_string, output,  NULL);   //execute render
        return;
      }
      else if (tabs_list[tab_count] > 0) {               //control returned to parent
        return;
      } 
      else {
        perror("fork problem\n");
        exit(-1);
      }
    }
  } 
  else { //On the blacklist
    printf("Url Entered is: %s. This is a Blacklisted URL \n", output);
    alert("BLACKLIST");
    return;
  }
 
  
  
  return; //Just in case something weird happened and we got out here
}
/* === STUDENTS IMPLEMENT=== */
/* 
    Function: init_blacklist --> "Open a passed in filepath to a blacklist of url's, read and parse into an array"
    Input:    char* fname    --> "file path to the blacklist file"
    Returns:  void
    Hints:
            (a) File I/O (fopen, fgets, etc.)
            (b) If we want this list of url's to be accessible elsewhere, where do we put the array? 
*/
void init_blacklist (char *fname) {
  //STUDENTS IMPLEMENT
  FILE *file;
  int max_url_exceeded = 0;         //flag set to 1 if MAX_BAD exceeded.
  int url_line = 0;
  file = fopen(fname, "r"); //set read permissions as we are only reading
  if(file == NULL) {
    printf("Error opening blacklist file.\n");
    return;
  }
  while(!feof(file) && !ferror(file) && !max_url_exceeded) { //loop through the file
    if(fgets(blacklisted_urls[url_line], MAX_URL, file) != NULL) {  //NOTE: this grabs '\n' too!
      url_line++;
      if(url_line > MAX_BAD) { //check to limit the blacklist number
        max_url_exceeded = 1;
      }
    } 
  }
  fclose(file);
  
  //For some reason, our entries had an additional newline, so we must remove it
  //for loop
  for(int j = 0; j < MAX_BAD; j++) {
      char* black = blacklisted_urls[j];
      black[strlen(black)-1] = '\0';
  }
  //Works as of 10/3/2022

}

/* === STUDENTS IMPLEMENT=== */
/* 
    Function: main 
    Hints:
            (a) Check for a blacklist file as argument, fail if not there [PROVIDED]
            (b) Initialize the blacklist of url's
            (c) Create a controller process then run the controller
                (i)  What should controller do if it is exited? Look at writeup (KILL! WAIT!)
            (d) Parent should not exit until the controller process is done 
*/
int main(int argc, char **argv)
{

  //STUDENT IMPLEMENT
  // (a) Check arguments for blacklist, error and warn if no blacklist
  if (argc != 2) {
    fprintf (stderr, "browser <blacklist_file>\n");
    exit (0);
  }
  //(b) Initialize the blacklist of url's
  init_blacklist("blacklist");

  //(c.) Create a controller process, then run the controller
    //(i.) What should the controller do if it is exited? Look at writeup (KILL! WAIT!)

  //run_control(); This was for intermediate submission, which worked properly
  
  child_controller = fork(); //fork controller
  if(child_controller == 0) { //Controller process
    run_control();
    for(int i = 0; i <= tab_count; i++){ //kill all
      kill(tabs_list[i], 9);
      wait(NULL);
    }
    //For every fork() there is a wait, so if you have to kill in a loop, you have to wait in a loop
  }
  else if (child_controller > 0) { //Here's where we do the wait and hold until everything is killed. Child and zombies alike
    wait(NULL);
    printf("Killed\n");
    //done!
  }
  else {
    perror("fork problem with controller\n");
    exit(-1);
  }

  free(www_input_str);
  free(http_input_str);
  free(https_input_str);
  return 0;
}
