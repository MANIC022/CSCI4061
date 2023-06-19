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
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    printf("Demo Code: pipe\n");

    /* (a) Open a pipe */
    int pipe_fds[2];
    int ret_val = pipe(pipe_fds);
    if(ret_val == -1){
        perror("error using pipe\n");
        exit(-1);
    }
    printf("(1) Success... opened a pipe\n");

    /* (b) Fork a Child Process that will sleep and then read from the pipe */
    if(fork() == 0){
        close(pipe_fds[1]);     // Child close the unused write side of the pipe
        printf("I am the child... reading from pipe... blocked until vals to read\n");
        int val_read;
        read(pipe_fds[0], &val_read, sizeof(int));
        printf("I am the child... I read [%d] from the pipe\n", val_read);
        close(pipe_fds[0]);      // Child close the read side, we are done with it now
        exit(0);
    }

    /* (c) Parent sleeps, then writes "15" to the pipe then cleans up  */
    close(pipe_fds[0]);         //Parent close the unused "read" end of the pipe
    printf("I am parent... sleeping for 1 second then writing [15] to pipe\n");
    sleep(1);
    int to_write = 15;
    write(pipe_fds[1], &to_write, sizeof(int));

    /* (d) Parent cleanup... close other FD, wait for child process, print and return */
    wait(NULL);
    close(pipe_fds[1]);         //Parent close the other end of the pipe, we are done
    printf("I am the parent... I am done!\n");

    return 0;
}




