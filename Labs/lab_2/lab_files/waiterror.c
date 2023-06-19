#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    pid_t pid;
    int wstatus;

    pid = fork();

    if (pid == 0) {
	char *argv[3] = {"ls", "doesnotexist", NULL};
	execvp( "ls", argv );
    } else {
        wait(&wstatus);
        if (WEXITSTATUS(wstatus) != 0) {
            printf("child exited with error code=%d\n", WEXITSTATUS(wstatus));
	    exit(-1);
        }
    }
}
