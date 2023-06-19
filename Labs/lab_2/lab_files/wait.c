#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main() {
	pid_t pid = fork();
	int status;
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
		wait(&status);
                if (WIFEXITED(status))
			printf("child exited with status %d\n", WEXITSTATUS(status));
		else
			fprintf(stderr, "child terminated abnormally\n");
	}
        return 0;
}
