#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
    printf("Demo Code: dup + dup2\n");

    printf("(1) Printing to fd[%d] --> Should be STDOUT\n", STDOUT_FILENO);

    /* (a) Duplicate STDOUT file number */
    int dup_stdout = dup(STDOUT_FILENO);
    if(dup_stdout == -1){
        perror("error using dup\n");
        exit(-1);
    }

    /* (b) Open a file so we can redirect to it */
    int fd = open("dup_output", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    /* (c) Re-Direct STDOUT to newly opened file */
    int ret = dup2(fd, STDOUT_FILENO);
    if(ret == -1){
        perror("error using dup2\n");
        exit(-1);
    }

    printf("(2) Printing to fd[%d] --> Should be dup_output now since we redirected stdout to fd\n", STDOUT_FILENO);

    /* (d) Restore STDOUT to it's original FD */
    ret = dup2(dup_stdout, STDOUT_FILENO);
    if(ret == -1){
        perror("error using dup2\n");
        exit(-1);
    }

    printf("(3) Printing to fd[%d] --> Should be back to stdout now\n", STDOUT_FILENO);

    /* (e) Cleanup and close */
    close(fd);
    return 0;
}




