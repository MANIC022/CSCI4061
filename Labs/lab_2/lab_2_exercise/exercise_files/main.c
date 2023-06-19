#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){

     
//  1. Takes an integer argument from the command line and store it in a variable. example. "num".
     // if argument is not provided, print error message
     if(argc == 1){
     	printf("Error: no argument provided from command line\n");
     	return -1;
     	}
     int number = atoi(argv[1]);
     if (number < 0) {
     	printf("Error: can't have negative numbers\n");
     	return -1;
     	}
     
// 2. Forks two children processes
     // a. First child computes and prints the first Nth terms of Fibonacci Sequence.  
     // and prints out the result and its own identifier ID (process id).
     // Example:   [ ID = 262750] The list of first 5 terms in the Fibonacci Sequence is: 0, 1, 1, 2, 3
     
     
     // b. Second child computes the factorial of Number (1*2*...*N1) and prints out
     // the result and its own identifier ID.
     // Example [ ID = 4076743] Factorial of 5 is 120
     
     //First define the key functions
     int* arr = (int *)malloc(sizeof(int)*number);
     int fib(int x, int* list){
     		list[0] = 0;
     		list[1] = 1;
     		for(int i = 2; i < number; i++){
     			list[i] = list[i-1]+list[i-2];
     		}
     		
     		return 0;
     	}
     
     int factorial(int y){
     	if(y <= 1){
     		return 1;
     	}
     	else{
     		return y * factorial(y-1);
     	}
     }
     
     //Next, do the forking
     pid_t pid = fork();
     int waitreturn;
     
     if (pid == -1) {
       perror("fork() failed");
       free(arr);
       exit(1);
     } 
     else if (pid == 0){
     	//print fibonnaci here
     	fib(number, arr);
     	printf("[ID = %d] The first %d terms in the Fibonnaci Sequence are: ", getpid(), number); 
     	for(int i = 0; i < number-1; i++){
     		printf("%d, ", arr[i]);
     	}
     	printf("%d", arr[number-1]);
     	printf("\n");
     	free(arr);
     	exit(0);
     }
     else{
     	pid_t childpid = fork();
     	int status;
     	
     	if (childpid == -1) {
                perror("fork() failed");
                exit(1);
        } else if (childpid == 0) {
		printf("[ID = %d] Factorial of %d is: %d\n", getpid(), number, factorial(number));
		exit(0);
			
	} else {
     		status = wait(&status);
     		waitreturn = wait(&waitreturn);
     		if (WIFEXITED(status) == 0 && WIFEXITED(waitreturn) == 0)
			printf("[ID = %d] Done\n", getpid());
		else
			printf("Abnormal Child Termination\n");
	}
	}
     return 0;
}
     
// 3. Parent waits until both children are finished, then prints out the message “Done”
     // and its own identifier.
     // Example: [ID = 4076741] DONE

