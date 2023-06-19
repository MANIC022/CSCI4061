#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
int main()
{

   	// Fork for process 1: readfile excecution
   	pid_t childpid1 = fork();
   	int childWaitStatus1;
   	
   	if(childpid1 == -1){
   		perror("fork() failed");
   		exit(1);
   	}
   	else if (childpid1 == 0){
   		//excecute file here as this is child 1
   		
   		execl("./readFile", "readFile", "file1.txt", NULL);
   		exit(0);
   	}
   	else {
   		// Fork for process 2: readfile2 execution
   		pid_t childpid2 = fork();
   		int childWaitStatus2;
   		if(childpid2 == -1) {
   			perror("fork() failed");
   			exit(1);
   		}
   		else if (childpid2 == 0){
   			//execute file here as this is child 2
   			execl("./readFile2", "readFile2", "file2.txt", NULL);
   			exit(0);
   		}
   		else {
   			//use defined ints to wait
   			childWaitStatus1 = wait(&childWaitStatus1);
     			childWaitStatus2 = wait(&childWaitStatus2);
     			if (WIFEXITED(childWaitStatus1) == 0 && WIFEXITED(childWaitStatus2) == 0){
     				//Continue here as it means first two children have succeeded
     				//Fork for process 3: mergedCount
     				pid_t childpid3 = fork();
     				int childWaitStatus3;
     				
     				if(childpid3 == -1){
     					perror("fork() failed");
     					exit(1);
     				}
     				else if(childpid3 == 0){
     					//execute file here as this is child 3
     					execl("./mergeCount", "mergeCount", "mergeCount.txt", NULL);
     					exit(0);
     				}
     				else{
     					childWaitStatus3 = wait(&childWaitStatus3);
     					if (WIFEXITED(childWaitStatus3) == 0){
     						//success, all have executed
     						
     						return 0;
     					}
     					else{
     						printf("Abnormal Child 3 Termination\n");
     						return -1;
     					}
     				
     				
     				}
     				
				//printf("[ID = %d] Done\n", getpid());
			}
			else
				{
				printf("Abnormal Child 1 or 2 Termination\n");
				return -1;
				}
		}
	}
	
    
    
    /*
   
    Program Instruction:
    You have 3 executables you need to execute.
    readFile
    readFile2
    mergedCount
    
    
    Write a program that will execute readFile and readFile2 in parallel.
    mergedCount must be executed after the first two executables are done executing.
    
    Note( readFile and readFile2 take in one command line arguement). 
    Pass in file1.txt as an arguement for readFile and pass in file2.txt as an arguement for readFile20
    
    You can check mergedCount.txt after the executables have been executed. It should have all the word that are common between file1.txt and file2.txt
    and it should have their word counts.
    */

   //return 0;
}
