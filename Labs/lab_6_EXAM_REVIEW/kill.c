#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

void infinteProcess(void){
    while(1){
        printf("Child running forever...\n");
        sleep(1);   /* Sleep 1 second */
    }
}

int main(int argc, char *argv[]) {
    printf("Demo Code: kill\n");

    /* (a) Fork a child and let it run forever */
    pid_t pid = fork();
    if(pid == 0){
        infinteProcess();
        printf("Child should not print this because killed!\n");
        exit(0);
    }

    /* (b) Parent sleep for 3 seconds and let child run */
    printf("Parent Sleeping 3 seconds...\n");
    sleep(3);

    /* (c) Attempt to kill child */
    printf("Parent Killing Child...\n");
    int ret_val = kill(pid, SIGKILL);
    if(ret_val == -1){
        perror("Kill Failure!\n");
        exit(-1);
    }

    /* (d) Indicate success and return */
    printf("Success! Child [%d] Killed\n", pid);
    wait(NULL);

    return 0;
}




