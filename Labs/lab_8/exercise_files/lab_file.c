

//1 pc 1 child
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include<sys/shm.h>

#define PERM 0666
#define MSGSIZE 100



//write code where parent write to memory, child overwrite it after reading 
//and parent should read again to get the value written by child

int main(void) {

    key_t key;
    int msgid;

    // generate unique key
    key = ftok("recitation", 4061);

    // creates a message queue and allocate memory
    if ((msgid = shmget(key, MSGSIZE ,PERM | IPC_CREAT)) < 0) {
        printf("Failed to create a queue\n");
        return -1;
    }

    char* shm;

    //attach the shared memory
    if ((shm=shmat(msgid,NULL,0))==(char *)-1){
        perror("Unable to attach the memory");
    }



    //the parent should write a lower case string and child converts it to upper case
    // you can use strcpy

    // 1) parent: write to memory
    char* test_string = "hello world";
    strcpy(shm, test_string);
    printf("Written to memory: %s\n", shm);

    // 2) child get the access to memory

    pid_t child = fork();
    if(child == -1){ //Fail case for child
        printf("Failed to create child\n");
        exit(1);
    }


    // 3) child overwrite the shared memory after reading
    else if(child == 0){ //Child
        char* read_memory = shm;
        printf("Before editing: %s\n", read_memory);

        for(int i = 0; i < strlen(read_memory); i++){
            if(read_memory[i] >= 'a' && read_memory[i] <= 'z'){
                read_memory[i] = read_memory[i] - 32;
            }
        }

        printf("After editing: %s\n", read_memory);

        exit(0);
    }

    else{ //Control returned to parent
    printf("waiting for the child...\n");

    wait(NULL);

    //get update string
    printf("Parent read: %s\n", shm);

    // to destroy the message queue
    shmctl(msgid, IPC_RMID, NULL);
    }
         
}

