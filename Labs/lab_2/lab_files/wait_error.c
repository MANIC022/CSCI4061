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
		execlp("ls", "ls", "wrong_path", NULL);
	} else {
		wait(&status);
                if (WIFEXITED(status) != 0)
			printf("child exited with status %d\n", WEXITSTATUS(status));
		else
			fprintf(stderr, "child terminated abnormally\n");
	}
        return 0;
}
