#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NTHREADS 10

int cur_queue_len = 0;   // shared variable

pthread_t add_thread[NTHREADS];     // pthread_t array for addQueue threads
pthread_t delete_thread[NTHREADS];  // pthread_t array for deleteQueue threads
// TODO: [Task3] Initialize lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



// increase cur_queue_len by 1
// TODO: [Task3] Synchronize the queue so that 1 thread can access critical section
void* addQueue () {
    pthread_mutex_lock(&lock);
    cur_queue_len++;
    printf("Added one item. Current queue length is: %d\n", cur_queue_len);
    fflush(stdout);
    pthread_mutex_unlock(&lock);
    return NULL;
}


// decrease cur_queue_len by 1
// TODO: [Task3] Synchronize the queue so that 1 thread can access critical section
void* deleteQueue() {
    pthread_mutex_lock(&lock);
    if(cur_queue_len == 0){
        printf("No item in the queue\n");
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    else{
        cur_queue_len--;
        printf("Deleted one item. Current queue length is: %d\n", cur_queue_len);
        fflush(stdout);
        pthread_mutex_unlock(&lock);
        return NULL;
    } 
}


int main() {
    int i;
    int arg_arr[NTHREADS];

    // TODO: [Task1] Create 20 threads. 10 threads execute “addQueue”. Other 10 threads execute “deleteQueue”.
    for (i = 0; i < NTHREADS; i++) { // Initialize array
        arg_arr[i] = i;
    }

    for(i = 0; i < NTHREADS; i++) {
        // Create threads
        if(pthread_create(&(add_thread[i]), NULL, addQueue, (void *) &arg_arr[i]) != 0) {
            printf("Thread %d  in add_thread failed to create\n", i);
        }
        if(pthread_create(&(delete_thread[i]), NULL, deleteQueue, (void *) &arg_arr[i]) != 0){
            printf("Thread %d  in delete_thread failed to create\n", i);
        }
    }


    // TODO: [Task2] Join 20 threads before main returns
    for(i = 0; i < NTHREADS; i++) {
        pthread_join(add_thread[i], NULL);
        pthread_join(delete_thread[i], NULL);
    }


    printf("Final queue len: %d\n", cur_queue_len);
    return EXIT_SUCCESS;
}
