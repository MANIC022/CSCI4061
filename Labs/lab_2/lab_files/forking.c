#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
pid_t childpid;

	printf("sizeof(pid_t) = %d\n", (int)sizeof(pid_t));
        childpid = fork();
        if (childpid == -1)
        {
                perror("fork() failed");
                return 1;
        }
        if (childpid == 0)
                printf("I am a child with id %ld\n", (long)getpid());
        else {
                printf("I am a parent with id %ld\n", (long)getppid());
		printf("childpid = %ld\n", (long)childpid);
	}
        return 0;
}
