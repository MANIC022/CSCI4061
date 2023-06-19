#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
int main()
{
	pid_t childpid;

 /*       childpid = fork();
	if (childpid == -1)
	{
		perror("fork() failed");
		return 1;
	}
        printf("childpid == %d\n",childpid);
	if (childpid == 0)
		printf("I am a child with id %ld\n", (long)getpid());
	else
		printf("I am a parent with id %ld\n", (long)getpid());
*/

	int i,n=3;
	for(i=0;i<n;i++){
		childpid = fork();
                //printf("i= %d \n",i);
		if (childpid == 0){
			printf("I am a child with id %ld\n", (long)getpid());			
			break;
			}else{
			printf("I am a parent with id %ld\n", (long)getpid());
		}	
	}

	return 0;
}


