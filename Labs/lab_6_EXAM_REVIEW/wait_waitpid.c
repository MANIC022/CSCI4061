#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define NUM_PROCS 3

/* Reusable Function to Fork NUM_PROCS processes and return their PID's */
pid_t* ForkProcesses(int num_procs)
{
    pid_t *pids = malloc(num_procs * sizeof(pid_t));
    for(int i = 0; i < num_procs; i++){
        pid_t temp_pid = fork();
        if (temp_pid == -1){
            perror("Fork Failed!\n");
        }else if(temp_pid == 0){
            printf("Child [%d] Started     --> PID [%d]\n", i, getpid());
            exit(i);    // exit, child does nothing
        }else{
            pids[i] = temp_pid;
        }
    }
    return pids;
}


int main(int argc, char *argv[]) {
    printf("Demo Code: wait & waitpid\n");

    pid_t *pids;

    /* (a) Demonstrate Standard Wait */
    printf("\nWait Demo #1: wait for all, disregard status\n");
    pids = ForkProcesses(NUM_PROCS);
    for(int i = 0; i < NUM_PROCS; i++){
        pid_t ret_pid = wait(NULL);
        if(ret_pid == -1){
            perror("wait error\n");
            exit(-1);
        }else{
            printf("Successfully Waited for Child [%d]\n", ret_pid);
        }
    }
    free(pids);

    /* (b) Demonstrate Standard Wait, but check status */
    printf("\nWait Demo #2: wait for all, disregard status\n");
    pids = ForkProcesses(NUM_PROCS);
    for(int i = 0; i < NUM_PROCS; i++){
        int local_status;
        pid_t ret_pid = wait(&local_status);
        if(ret_pid == -1){
            perror("wait error\n");
            exit(-1);
        }else{
            /* Status packs in the exit status AND the reason for exit
                    --> WIFEXITED checks if the process exited normally (aka called exit)
                    --> WEXITSTATUS extracts the exit status from the return status of the process
            */
            if(WIFEXITED(local_status)){
                printf("Successfully Waited for Child [%d] --> Ret Code [%d]\n", ret_pid, WEXITSTATUS(local_status));
            }
            else{
                perror("Process did not exit normally, thus we cannot call WEXITSTATUS on it\n");
                exit(-1);
            }
        }
    }
    free(pids);

        /* (b) Demonstrate Standard Wait, but check status */
    printf("\nWaitpid Demo #3: calling wait PID\n");
    pids = ForkProcesses(NUM_PROCS);
    for(int i = 0; i < NUM_PROCS; i++){
        int local_status;
        /* Typically use NO flags or options when using waitPID unless you explicitly want to not block */
        pid_t ret_pid = waitpid(pids[i], &local_status, 0);
        if(ret_pid == -1){
            perror("wait error\n");
            exit(-1);
        }else{
            if(WIFEXITED(local_status)){
                printf("Successfully Waited for Child [%d] --> Ret Code [%d]\n", ret_pid, WEXITSTATUS(local_status));
            }
            else{
                perror("Process did not exit normally, thus we cannot call WEXITSTATUS on it\n");
                exit(-1);
            }
        }
    }
    free(pids);

    printf("\nWait Demo #4: Shorthand method to wait for all children\n");
    pids = ForkProcesses(NUM_PROCS);
    int child_wait_count = 0;
    while(wait(NULL) > 0){
        child_wait_count++;
    }
    printf("Successfully waited for [%d] children\n", child_wait_count);
    free(pids);

    return 0;
}