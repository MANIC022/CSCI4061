#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int killed = 0;

void handle_SIGINT(int sig_num) {
  // TODO: Write COde for Handler for Child
  signal(SIGINT, handle_SIGINT);
  printf("\nNo SIGINT-erruptions allowed.\n");
  fflush(stdout);
}
void handle_SIGINTParent(int sig_num) {
  //TODO : Write code for the Handler for Parent
  signal(SIGINT, handle_SIGINTParent);
  printf("\nParent is killed, sending SIGKILL to child.\n");
  killed = 1;
  fflush(stdout);
}
int main(int argc, char *argv[]){

  int pid = fork();                          // fork execution

  if(pid == 0){                              // CHILD
    // TODO Catch SIGINTs thrown at the child here. Enter code here
    signal(SIGINT, handle_SIGINTParent);

    int num=1;
    while(1){
      num+=1;
      printf("Current num is %d\n", num);
    }    // execute specified program
  }

  // TODO: Write Code to catch SIGINT for Parent
  signal(SIGINT, handle_SIGINTParent);
  int status, ret; 
  int result=0;                          
  while(1){             // Loop until child done
    sleep(3);          // sleep for given 3 seconds
    
    // Todo: Send SIGKILL if Parent receives a KeyBoard Interrupt( SIGINT)
    if(killed){
      printf("Killing child\n");
      kill(pid, SIGKILL);
    }




    ret = waitpid(pid, &status, WNOHANG);    // check on child
    if(ret == pid){                          // if child is finished
      break;                                 //   break from loop
    }
    result = kill(pid,SIGINT);           // send a interrupt signal to child
    printf("kill result: %d\n",result);      // check on delivery
  }
    
  if(WIFSIGNALED(status)){                   // check if a signal ended the child
    printf("child process %d terminated with signal %d\n",
           pid,WTERMSIG(status));
  }

  exit(0);
}