
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char *argv[])
{
	pid_t childpid;
	//argv[1] = "ps -a -f";	

	childpid = fork();
	if(childpid==-1){
		perror("Failed to fork");
		return 1;
	}

	//child code
	if(childpid == 0){
		execl("/bin/ps", "ps", "-af",NULL);
		perror("child failed to exec all_ids");
		return 1;
	}

	if(childpid != wait(NULL)){
		perror("parent failed to wait due to signal or error");
                return 1;

	}
	return 0;
}
