#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
int main()
{
	pid_t childpid;

        childpid = fork();
	if (childpid == -1)
	{
		perror("fork() failed");
		return 1;
	}
        printf("childpid == %d\n",childpid);
	if (childpid == 0){
		printf("I am a child with id %ld\n", (long)getpid());
		printf("Terminating process %ld \n", (long)getpid());
       		kill((long)getpid(), 9);
	}else{
		printf("I am a parent with id %ld\n", (long)getpid());
		printf("Terminating process %ld \n", (long)getpid());
       		kill((long)getpid(), 9);
	}
	return 0;
}
