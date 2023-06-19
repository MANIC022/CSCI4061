#include "matmult.h"

/* TODO you will need to initialize 2 conditions and 1 lock */
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t buffer_access = PTHREAD_MUTEX_INITIALIZER;



#define MAX_BUFF_SIZE 7
int* bounded_buffer[MAX_BUFF_SIZE];
int CUR_BUF_SIZE;

matrix_t matrix_1;
matrix_t matrix_2;
matrix_t multi_thread_answer;

void *multiplier_thread(void *arg)
{
    //Figure out which column this thread is in charge of
    int col_index =  *(int*)arg;

    int *local = malloc(sizeof(int) * multi_thread_answer.NUM_ROW * multi_thread_answer.NUM_CLM);

    //Compute a portion of the matrix multiplication locally
    for(int i = 0; i < multi_thread_answer.NUM_ROW; i++){
        for(int j = 0; j < multi_thread_answer.NUM_CLM; j++){
            local[(i*multi_thread_answer.NUM_CLM) + j] = matrix_1.data[i][col_index] * matrix_2.data[col_index][j];
        }
    }

    /* TODO conditionally access your buffer
        (a) Do you need a lock?
        (b) What condition do you check here? 
        (c) If the condition from "b" is not met, what condition variable should you wait to be signaled? 
    */
    // (a) Yes, we need the lock
    pthread_mutex_lock(&buffer_access);

    // (b) We need to check the buffer is full condition
    while (CUR_BUF_SIZE == MAX_BUFF_SIZE) {
		//(c) Buffer full, wait for buffer_not_full to be signaled
		pthread_cond_wait(&buffer_not_full, &buffer_access);
	}

    /* TODO buffer is now NOT empty, what should you signal? What should you release? */
    //We do the regularly scheduled thing, and then signal buffer is not empty and release the lock
    bounded_buffer[CUR_BUF_SIZE] = local;
    CUR_BUF_SIZE++;

    //Unlock at end
    pthread_cond_signal(&buffer_not_empty);		
	pthread_mutex_unlock(&buffer_access);

}

void *adder_thread(void *arg)
{
    int number_of_additions = 0;
    /* Loop for column number of additions */
    while(number_of_additions < matrix_1.NUM_CLM)
    {
        /* TODO conditionally access your buffer
            (a) Do you need a lock?
            (b) What condition do you check here? 
            (c) If the condition from "b" is not met, what condition variable should you wait to be signaled? 
        */
       
       // (a) Yes we need the lock
       pthread_mutex_lock(&buffer_access);

       // (b) We must check if the buffer is empty
       while (CUR_BUF_SIZE == 0) {
			//Buffer empty, release lock and sleep until buffer not empty
            // (c) We wait for buffer not empty signal
			pthread_cond_wait(&buffer_not_empty, &buffer_access);
		}

        /* Empty the buffer by performing additions into the answer matrix */
        for(int buf_index = 0; buf_index < CUR_BUF_SIZE; buf_index++)
        {
            int *cur_buf_val = bounded_buffer[buf_index];
            for(int i = 0; i < multi_thread_answer.NUM_ROW; i++)
            {
                for(int j = 0; j < multi_thread_answer.NUM_CLM; j++)
                {
                    multi_thread_answer.data[i][j] += cur_buf_val[(i * multi_thread_answer.NUM_CLM) + j];
                }
            }
            free(cur_buf_val);
            number_of_additions++;
        }
        CUR_BUF_SIZE = 0;

        /* TODO buffer is now empty, what should you signal? What should you release? */
        //Signal that the buffer is not full and unlock the access
        pthread_cond_signal(&buffer_not_full);		
		pthread_mutex_unlock(&buffer_access);    
    }
}

int main(int argc, char **argv){

    //Random seed based on time of day
    srand(time(NULL));

    //Init and fill matrix 1 size(DIM1 x DIM2)
    matrix_1.NUM_ROW    = DIM1;
    matrix_1.NUM_CLM    = DIM2;
    fill_matrix(&matrix_1);

    //Init and fill matrix 2 size(DIM2 x DIM1)
    matrix_2.NUM_ROW    = DIM2;
    matrix_2.NUM_CLM    = DIM1;
    fill_matrix(&matrix_2);

    //Retrieve Single Thread Answer (correct answer)
    printf("\n=============== Correct Answer ==================\n"); 
    matrix_t one_thread_answer = single_thread_matmult(matrix_1, matrix_2);
    printf("====================================================\n\n");

    //Initialize multi_thread answer matrix
    multi_thread_answer.NUM_ROW = matrix_1.NUM_ROW;
    multi_thread_answer.NUM_CLM = matrix_2.NUM_CLM;
    zero_matrix(&multi_thread_answer);


    //Create "Adder" Thread
    pthread_t tid_adder;
    if(pthread_create(&tid_adder, NULL, adder_thread, NULL)!=0){
        fprintf(stderr,"Error creating adder thread\n");
        exit(1);
    }

    //Create multiplier threads
    pthread_t multiplier_threads[matrix_1.NUM_CLM];  
    int index_arr[matrix_1.NUM_CLM];
    for(int i = 0; i < matrix_1.NUM_CLM; i++){
        index_arr[i] = i;
        if(pthread_create(&multiplier_threads[i], NULL, multiplier_thread, &index_arr[i])!=0){
            fprintf(stderr,"Error creating multiplier thread\n");
            exit(1);
        }
    }

    //Join adder thread
    if(pthread_join(tid_adder, NULL)!= 0){
        fprintf(stderr,"Error joining verification thread\n");
        exit(1);
    }

    //Verify the matrix multiplication was correct
    verify_matrix(one_thread_answer, multi_thread_answer);
    
    //Join multiplier threads
    for(int i = 0; i < matrix_1.NUM_CLM; i++){
       if(pthread_join(multiplier_threads[i], NULL)!= 0){
            fprintf(stderr,"Error joining multiplier thread\n");
            exit(1);
       }
    }

    //Free memory
    free_matrix(matrix_1);
    free_matrix(matrix_2);
    free_matrix(one_thread_answer);
    free_matrix(multi_thread_answer);

    return 0;
}


