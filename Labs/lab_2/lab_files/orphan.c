#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	pid_t pid = fork();
        if (pid == -1) {
                perror("fork() failed");
                exit(1);
        } else if (pid == 0) {
                printf("I am child process with pid=%d, ppid=%d\n", getpid(), getppid());
		sleep(10);
                printf("I am child process with pid=%d, ppid=%d\n", getpid(), getppid());
	} else {
                printf("I am parent process with pid=%d, ppid=%d\n", getpid(), getppid());
                printf("My child pid=%d\n", pid);
	}
        return 0;
}
