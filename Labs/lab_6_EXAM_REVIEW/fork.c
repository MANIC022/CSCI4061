#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
    printf("Demo Code: fork\n");

    /* (a) Fork a child process */
    pid_t pid = fork();
    
    /* (b) Check for fork error */
    if(pid == -1){
        perror("fork error\n");
        exit(-1);
    }

    /* (c) Demonstrate Child Code */
    if(pid == 0){
        printf("CHILD:  Value from fork call [%07d] - My Process ID [%07d]\n", pid, getpid());
        return 0;
    }
    
    /* (d) Demonstrate Parent Code */
    else{
        printf("PARENT: Value from fork call [%07d] - My Process ID [%07d]\n", pid, getpid());
    }

    /* (e) Parent waits for child and returns */
    wait(NULL);
    return 0;
}




