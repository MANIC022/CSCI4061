#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

main()
{
    pid_t child_id;
    int stat_loc;

    if ((child_id = fork())) /* this is the parent process */
    {
       printf("PARENT: Created a child process\n");
       printf("PARENT:   will wait till my child with pid %d finishes\n", child_id);
       child_id = wait(&stat_loc);
       printf("PARENT: My child with pid %d is done.\n", child_id);
       
    }
    else    /* this is the child process */
    {
        printf("CHILD: I am the newly created child process\n");
        printf("CHILD: Now I will execute a different program\n");
        execl ("./child", "red", "white", "blue",  NULL);
	
        printf("Hello! Are you there?\n");
      
    }
}
