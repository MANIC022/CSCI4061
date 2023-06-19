#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main() {
	pid_t pid = fork();
        if (pid == -1) {
                perror("fork() failed");
                exit(1);
        } else if (pid == 0) {
	} else {
		sleep(60);
	}
        return 0;
}
