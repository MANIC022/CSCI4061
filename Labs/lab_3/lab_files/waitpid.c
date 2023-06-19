#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void main( int argc,  char*  argv[] )
{
    int cpid;
    int stat;
    int retval;

    if (cpid = fork()) /* this is the parent process */
    {
       printf("PARENT: Created a child process\n");
       printf("PARENT: pid for the child process %d\n", cpid);
       while (retval != cpid ) {
          retval = waitpid( cpid, &stat, WNOHANG);    
          printf("PARENT: return value is %d \n", retval );
          sleep(1);
       }

    }
    else    /* this is the child process */
    {
        printf("CHILD: I am the newly created child process\n");
        printf("CHILD: Now I will execute a different program\n");
        execl ("./child", "red", "white", "blue",  NULL);
     
        printf("Hello! Are you there?\n");
      
    }
}
