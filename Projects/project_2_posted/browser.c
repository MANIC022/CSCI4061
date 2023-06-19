/* CSCI-4061 Fall 2022
 * Group Member #1: <Amit Manicka> <manic022>
 * Group Member #2: <Aditya Prabhu> <prabh079>
 * Group Member #3: <Anton Priborkin> <pribo002>
 * 
 * Contributions:
 * Amit wrote get_num_tabs(), get_free_tab(), init_tabs(), non_block_pipe(), handle_uri(), uri_entered_cb(), new_tab_created_cb(), menu_item_selected_cb(), run_control(), and main()
 * Aditya wrote fav_ok(), update_favorites_file(), init_favorites(), uri_entered_cb(), run_control(), and main()
 * Anton wrote fav_ok(), update_favorites_file(), init_favorites()
 */

#include "wrapper.h"
#include "util.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <math.h>

#define MAX_TABS 100  // this gives us 99 tabs, 0 is reserved for the controller, so that makes total of 99+1 = 100
#define MAX_BAD 1000  // MAX number on blacklist
#define MAX_URL 100   // MAX length of URLS
#define MAX_FAV 100   // MAX number of favorites stored
#define MAX_LABELS 100 
/************************/
/* Constants Declared by Team */
/************************/
#define MSGSIZE 100                       //Pipe Message Size

comm_channel comm[MAX_TABS];                 // Communication pipes
char favorites[MAX_FAV][MAX_URL];    // Maximum char length of a url allowed
int num_fav = 0;                          // # favorites

typedef struct tab_list {
  int free;
  int pid; // may or may not be useful
} tab_list;

// Tab bookkeeping
tab_list TABS[MAX_TABS];  

int error_flag_for_favs = 0; // Relevant for error checks

/************************/
/* Simple tab functions */
/************************/

// return total number of tabs
// 0 = Not free (in use and can render something)
// 1 = Free (not rendering anything)
int get_num_tabs () {
  int counter = 0; //number of tabs
  for(int i = 1; i < MAX_TABS; i++){
    if(TABS[i].free == 0){ //If tab is not free, it is open and being used
      counter++;
    }
  }
  return counter; //return the number of not free tabs, or the total number of tabs
}

// get next free tab index
int get_free_tab () {
  int free_index = -1; //Means we didn't find anything
  for(int i = 1; i < MAX_TABS; i++){ //Run through the entire array of tabs
    if(TABS[i].free == 1){ //Found a free one
      free_index = i;
      break;
    }
  }
  return free_index; //Returns first free one
}

// init TABS data structure
void init_tabs () {
  int i;

  for (i=1; i<MAX_TABS; i++)
    TABS[i].free = 1;
  TABS[0].free = 0;
}

/***********************************/
/* Custom Functions (Created by Team)*/
/***********************************/

/***********************************/
/* Favorite manipulation functions */
/***********************************/

// return 0 if favorite is ok, -1 otherwise
// at max limit => return -1, already a favorite => return -2 (Hint: see util.h)
int fav_ok (char *uri) {
  if (num_fav >= MAX_FAV) {             //Check if max favorite limit is reached
    return -1;                          //Error code -1 => max limit reached
  }
  else {                                //If limit not reached, check if favorite already added.
    if(on_favorites(uri))
      return -2;                        //Error code -2 => favorite already exists
  }
  return 0;                             //If program made it to this point, return 0 (success).
}


// Add uri to favorites file and update favorites array with the new favorite
void update_favorites_file (char *uri) {
  error_flag_for_favs = 0; //Resets the flag
  // Error checking to see if max size 100 hit
  int fav_ok_status = fav_ok(uri);
  if (fav_ok_status == -1) {                      //Error code -1 => max limit reached.
    alert("MAX FAVORITES REACHED");
    printf("Error: Favorites limit reached.\n");
    error_flag_for_favs = 1;                      //Sets the flag
    return;
  }
  else if (fav_ok_status == -2) {                 //Error code -2 => favorite already exists
    printf("Error: Favorite already exists.\n");
    alert("FAVORITE ALREADY EXISTS");
    error_flag_for_favs = 1;                      //Sets the flag
    return;
  } 
  else {                                          //Success => Update favorites
    FILE *file;
    file = fopen(".favorites", "a");              //Open favorites in APPEND mode
    if (file == NULL) {                           //Error checking for system calls
      printf("Error opening favorites file.\n");
      error_flag_for_favs = 1;                    //Sets the flag
      exit(1);  
    }


    if(fprintf(file, "%s\n", uri) >= 0) {         //Error checking for system calls (fprintf returns neg value if problem)
      sprintf(favorites[num_fav],"%s", uri);      //Append URI to favorites array
      fclose(file);                               //Close favorites file - done with it
      num_fav++;                                  //Increment favorites counter.
    }

    
    else {
      printf("Error appending to favorites file.\n");
      error_flag_for_favs = 1;                    //Sets the flag
      exit(1);
    }
  }
  return;                                         //If reached here, uri successfully added.
}

// Set up favorites array
void init_favorites (char *fname) { 
  FILE *file;                                     //file descriptor returned for favorites file
  int max_fav_exceeded = 0;                       //flag that becomes 1 once fav_line > MAX_FAV
  file = fopen(fname, "r");                       //Open .favorites file
  if(file == NULL) {                              //Error checking for system calls
    printf("Error opening favorites file.\n");               
    return;                         
  }
  while(!feof(file) && !ferror(file) && !max_fav_exceeded) {  //While not eof, 
    if(fgets(favorites[num_fav], MAX_URL, file) != NULL) {    //Error checking for system calls
      num_fav++;
      if(num_fav > MAX_FAV) {
        max_fav_exceeded = 1;                                 //While loop will no longer consider any URLs past MAX_FAV
      }
    }
    else {
    	printf("Either all favorites have been read and initialized, or there was an error midway\n");
    	break;
    }
  }
  fclose(file);
  //Get rid of Newline collected by fgets
  for(int i=0; i < num_fav; i++) {
    char *fav = favorites[i];                     //Have fav pointed to each favorited_url
    favorites[i][strlen(fav)-1] = '\0';           //Replace newline with null terminator
  }

}

// Make fd non-blocking just as in class!
// Return 0 if ok, -1 otherwise
// Really a util but I want you to do it :-)
int non_block_pipe (int fd) { //Makes any pipe nonblocking, important for the implementations that use it
  int nFlags;
  if ((nFlags = fcntl(fd, F_GETFL, 0)) < 0)
    return -1;
  if ((fcntl(fd, F_SETFL, nFlags | O_NONBLOCK)) < 0)
    return -1;
  return 0;
}

/***********************************/
/* Functions involving commands    */
/***********************************/

// Checks if tab is bad and url violates constraints; if so, return.
// Otherwise, send NEW_URI_ENTERED command to the tab on inbound pipe
void handle_uri (char *uri, int tab_index) {
  printf("uri entered is %s\n", uri);
  if(bad_format(uri)){    //Bad format check
    alert("BAD FORMAT");
    return;
  }
  if(on_blacklist(uri)){  //On blacklist check
    alert("BLACKLIST");
    return;
  }
    
  if(tab_index >= MAX_TABS || tab_index <= 0){ //check if index is valid
    alert("BAD TAB");
    return;
  }
  if(TABS[tab_index].free == 1){ //check if index tab is free to open
    alert("BAD TAB");
    return;
  }

  if(tab_index == 0){ //Ensure we do not overwrite the controller
    alert("BAD TAB");
    return;
  }
      
  else{
    //Send the NEW_URI_ENTERED command to tab
    req_t new_request;
    new_request.tab_index = tab_index;
    new_request.type = NEW_URI_ENTERED;
    strcpy(new_request.uri, uri);

    if(write(comm[tab_index].inbound[1], &new_request, sizeof(req_t)) == -1){ //Check error
      perror("failed to send the open request\n");
      return;
    }
    return;
  } 
}



// A URI has been typed in, and the associated tab index is determined
// If everything checks out, a NEW_URI_ENTERED command is sent (see Hint)
// Short function
void uri_entered_cb (GtkWidget* entry, gpointer data) {
  if(data == NULL) {	
    alert("NOTHING ENTERED");
    return;
  }
  // Get the tab (hint: wrapper.h)
  int tab = query_tab_id_for_request(entry, data);
  // Get the URL (hint: wrapper.h)
  char* uri = get_entered_uri(entry);
  // Hint: now you are ready to handle_the_uri
  handle_uri(uri, tab);

}
  

// Called when + tab is hit
// Check tab limit ... if ok then do some heavy lifting (see comments)
// Create new tab process with pipes
// Long function
void new_tab_created_cb (GtkButton *button, gpointer data) {
  if (data == NULL) {
    return;
  }

  // at tab limit?
  int tab_current_num = get_num_tabs();
  if(tab_current_num >= MAX_TABS){
    alert("MAX TABS REACHED");
    return;
  }

  // Get a free tab
  int free_tab_idx = get_free_tab();

  // Any tabs able to render? Also serves as a double check for max tabs.
  if(free_tab_idx == -1){
    alert("NO FREE TABS");
    return;
  }

  // Create communication pipes for this tab
  int ends_outbound[2];
  int ends_inbound[2];
  if(pipe(ends_outbound) == -1) {            //return fds to the r/w ends
    perror("outbound pipe error");           //error-checking for system calls
    exit(1);
  }             
  if(pipe(ends_inbound) == -1) {             //return fds to the r/w ends
    perror("inbound pipe error");            //error-checking for system calls
    exit(1);
  } 
          
  //OUTBOUND PIPE (Child -> Parent)
  comm[free_tab_idx].outbound[0] = ends_outbound[0]; //read end, Parent reads
  comm[free_tab_idx].outbound[1] = ends_outbound[1]; //write end, Child writes
  //INBOUND PIPE  (Parent -> Child)
  comm[free_tab_idx].inbound[0] = ends_inbound[0];  //read end, Child reads
  comm[free_tab_idx].inbound[1] = ends_inbound[1];  //write end, Parent writes

  // Make the read ends non-blocking 
  int outbound_flags = fcntl(comm[free_tab_idx].outbound[0],F_GETFL,0); //get current outbound flags
  if(fcntl(comm[free_tab_idx].outbound[0],F_SETFL, outbound_flags | O_NONBLOCK) == -1){
    perror("Failure to establish pipe conditions\n");
    return;
  } //add nonblocking flag to current flags

  int inbound_flags = fcntl(comm[free_tab_idx].inbound[0], F_GETFL,0);  //get current inbound flags
  if(fcntl(comm[free_tab_idx].inbound[0], F_SETFL, inbound_flags | O_NONBLOCK) == -1){
    perror("Failure to establish pipe conditions\n");
    return;
  } //add nonblocking flag to list

  // fork and create new render tab
  // Note: render has different arguments now: tab_index, both pairs of pipe fd's
  // (inbound then outbound) -- this last argument will be 4 integers "a b c d"
  // Hint: stringify args
  // Controller parent just does some TABS bookkeeping

  //1. Stringify args
  char render_args[20];
  sprintf(render_args, "%d %d %d %d", ends_inbound[0], ends_inbound[1], ends_outbound[0], ends_outbound[1]);
  char tab_num_as_string[4];
  sprintf(tab_num_as_string, "%d", free_tab_idx);

  //2. Fork and render the new tab
  pid_t new_window = fork();
  if(new_window < 0){
    perror("Error making new tab render\n");
    exit(0);
  }
  else if(new_window == 0){ //child process, time to render
    execl("./render", "render", tab_num_as_string, render_args, NULL);
    printf("Error in rendering execution\n");
    return;
  }
  else{ //3. Controller does bookeeping
    TABS[free_tab_idx].free = 0; //declare that that tab is now ready to render
    TABS[free_tab_idx].pid = new_window;

    return;
  }
  
}

// This is called when a favorite is selected for rendering in a tab
// Hint: you will use handle_uri ...
// However: you will need to first add "https://" to the uri so it can be rendered
// as favorites strip this off for a nicer looking menu
// Short
void menu_item_selected_cb (GtkWidget *menu_item, gpointer data) {
  if (data == NULL) {
    return;
  }
  
  // Note: For simplicity, currently we assume that the label of the menu_item is a valid url
  // get basic uri
  char *basic_uri = (char *)gtk_menu_item_get_label(GTK_MENU_ITEM(menu_item));

  // append "https://" for rendering
  char uri[MAX_URL];
  sprintf(uri, "https://%s", basic_uri);

  // Get the tab (hint: wrapper.h)
  int tab_number = query_tab_id_for_request(menu_item, data);

  // Hint: now you are ready to handle_the_uri
  handle_uri(uri, tab_number);

  return;
}


// BIG CHANGE: the controller now runs an loop so it can check all pipes
// Long function
int run_control() {
  browser_window * b_window = NULL;
  int i, nRead;
  req_t req;

  //Create controller window
  create_browser(CONTROLLER_TAB, 0, G_CALLBACK(new_tab_created_cb),
		 G_CALLBACK(uri_entered_cb), &b_window, comm[0]);

  // Create favorites menu
  create_browser_menu(&b_window, &favorites, num_fav);
  
  while (1) {
    process_single_gtk_event();
    // Read from all tab pipes including private pipe (index 0)
    // to handle commands:
    // PLEASE_DIE (controller should die, self-sent): send PLEASE_DIE to all tabs
    // From any tab:
    //    IS_FAV: add uri to favorite menu (Hint: see wrapper.h), and update .favorites
    //    TAB_IS_DEAD: tab has exited, what should you do?

    // Loop across all pipes from VALID tabs -- starting from 0
    for (i=0; i<MAX_TABS; i++) {
      if (TABS[i].free) continue;
      
      nRead = read(comm[i].outbound[0], &req, sizeof(req_t));

      // Check that nRead returned something before handling cases
      if(nRead == -1){
        continue;
      }
      
      // Case 1: PLEASE_DIE
      if(req.type == PLEASE_DIE){ //By definition this commmand has to come from the controller
        //Send to all tabs
        for(int j = 1; j < MAX_TABS; j++){
          if(TABS[j].free) //Tab wasn't open to begin with
            continue;

          //Generate kill request
          req_t new_req;
          new_req.type = PLEASE_DIE;
          new_req.tab_index = j;

          //Check if sending it via the comms will fail
          if(write(comm[j].inbound[1], &new_req, sizeof(req_t)) == -1){
            perror("Failure to kill tab\n");
            exit(-1);
          }
          if(wait(NULL) == -1){
            perror("Waiting has failed\n");
            exit(-1);
          }
        }
        exit(0);
      }
      // Case 2: TAB_IS_DEAD
      if(req.type == TAB_IS_DEAD){
        if(wait(NULL) == -1){
          perror("Wait on tab termination failed\n");
          exit(-1);
        }
        TABS[i].free = 1;
      }
      // Case 3: IS_FAV
      if(req.type == IS_FAV){
        update_favorites_file(req.uri);                 //Add uri to favorites file.
        if(error_flag_for_favs == 0){
          add_uri_to_favorite_menu (b_window, req.uri);  //Add to favorites menu. This command happens only if the error flag didn't trip
        }
      }

    }

  }
  usleep(1000);
  return 0;
}

//******************************************//
//             main() starts here           //
//******************************************//

int main(int argc, char **argv)
{

  if (argc != 1) {
    fprintf (stderr, "browser <no_args>\n");
    exit (0);
  }

  init_tabs ();
  //1. init blacklist (see util.h), and favorites
  init_blacklist(".blacklist");
  init_favorites(".favorites");

  // Fork controller
  // Child creates a pipe for itself comm[0]
  // then calls run_control ()
  // Parent waits ...

  pid_t child_controller = fork(); //2. fork controller
  if(child_controller == 0) { //Controller process
    //3. Create pipes here
    int controller_read = pipe(comm[0].outbound);
    int controller_write = pipe(comm[0].inbound);

    
    if(controller_read == -1){
    	printf("Failed to establish pipe\n");
    	return -1;
    }
    
    else if(controller_write == -1){
    	printf("Failed to establish pipe\n");
    	return -1;
    }

    if(non_block_pipe(comm[0].outbound[0]) == -1){
      printf("failed to set conditions for controller's pipes\n");
      return -1;
    }

    if(non_block_pipe(comm[0].inbound[0]) == -1){
      printf("failed to set conditions for controller's pipes\n");
      return -1;
    }
    
    run_control();


  }
  else if (child_controller > 0) { //Controller has terminated, so we end up here
    if(wait(NULL) == -1){
      perror("Ending wait has failed\n");
      exit(-1);
    }
    printf("Killed\n");
    exit(0);
  }
  else {
    perror("fork problem with controller\n");
    exit(-1);
  }

}
