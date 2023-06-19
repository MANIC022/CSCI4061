/* Exercise 3.21 Text Book Page no 76 */


#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char *argv[])
{
	pid_t childpid;
	pid_t waitreturn;
	int status;
	childpid = fork();
	
	if(childpid==-1)
	{
		perror("fork");
		exit(0);
		
	}	

	else if(childpid==0){
		printf("I am a child\n");
		exit(3);
	}

	else {
		waitreturn = wait(&status);
		if(WIFEXITED(status)) {

			printf("child exited with status %d\n",WEXITSTATUS(status));
		}
	
	}
}
