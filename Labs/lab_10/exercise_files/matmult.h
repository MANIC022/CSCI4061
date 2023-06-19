#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>

#define DIM1 5
#define DIM2 7

#define NUMDIGPRNT 3
#define NUMDIGPRNTANS 4

#define MAT_MAX 10
#define MAT_MIN 1

#define new_max(x,y) (((x) >= (y)) ? (x) : (y))
#define new_min(x,y) (((x) <= (y)) ? (x) : (y))

typedef struct{
    int NUM_ROW;
    int NUM_CLM;
    int **data;
} matrix_t;


/* NOTE: I do not recommend writing functions in header files, 
this is just to make the code cleaner in the exercise
*/

static void print_matrix(matrix_t to_print){
    for(int i = 0; i < to_print.NUM_ROW; i++){
        for(int j = 0; j <  to_print.NUM_CLM; j++){
            printf("%*d ", NUMDIGPRNT, to_print.data[i][j]);
        }
        printf("\n");
    }
}

static void fill_matrix(matrix_t *to_fill){
    to_fill->data = malloc(to_fill->NUM_ROW * sizeof(int*));
    for(int i = 0; i < to_fill->NUM_ROW; i++){
        to_fill->data[i] = (int*)malloc(to_fill->NUM_CLM * sizeof(int));
        for(int j = 0; j < to_fill->NUM_CLM; j++){
            to_fill->data[i][j] = rand() % MAT_MAX + MAT_MIN;
        }
    }
}

static void zero_matrix(matrix_t *to_zero){
    to_zero->data = malloc(to_zero->NUM_ROW * sizeof(int*));
    for(int i = 0; i < to_zero->NUM_ROW; i++){
        to_zero->data[i] = (int*)malloc(to_zero->NUM_CLM * sizeof(int));
        for(int j = 0; j < to_zero->NUM_CLM; j++){
            to_zero->data[i][j] = 0;
        }
    }
}

static void free_matrix(matrix_t to_free){
    for(int i = 0; i < to_free.NUM_ROW; i++){
        free(to_free.data[i]);
    }
    free(to_free.data);
}

void verify_matrix(matrix_t one_t_answer, matrix_t multi_t_answer)
{
    //Acquire the answer lock to be safe and sum the results
    bool was_match = true;
    for(int i = 0; i < one_t_answer.NUM_ROW; i++){
        for(int j = 0; j < one_t_answer.NUM_CLM; j++){
            if(one_t_answer.data[i][j] != multi_t_answer.data[i][j]){
                printf("ERROR! Matrix mismatch at index (%d,%d) Good Value [%d] Bad Value [%d]\n", i, j, one_t_answer.data[i][j], multi_t_answer.data[i][j]);
                was_match = false;
            }
        }
    }

    if(!was_match){
        printf("ERROR: Matrices did not match, race condition detected\n");
        printf("\n=============== WRONG Answer ====================\n");    
        print_matrix(multi_t_answer);
        printf("====================================================\n\n");
    }else{
        printf("SUCCESS: Matrices matched\n");
        printf("\n=============== YOUR  Answer ======================\n");    
        print_matrix(multi_t_answer);
        printf("====================================================\n\n");
    }

}

matrix_t single_thread_matmult(matrix_t mat1, matrix_t mat2){
    matrix_t answer;
    answer.NUM_ROW = mat1.NUM_ROW;
    answer.NUM_CLM = mat2.NUM_CLM;
    zero_matrix(&answer);

    //Compute Answer for printing
    for(int i = 0; i < mat1.NUM_ROW; i++){
        for(int j = 0; j < mat2.NUM_CLM; j++){
            for(int ii = 0; ii < mat1.NUM_CLM; ii++){
                answer.data[i][j] += mat1.data[i][ii] * mat2.data[ii][j];
            }
        }
    }

    for(int i = 0; i < new_max(mat1.NUM_ROW, mat2.NUM_ROW); i++){
        int space_offset = ((NUMDIGPRNT + 1) * (mat1.NUM_CLM + 1)) +2;
        if(i < mat1.NUM_ROW){
            space_offset = NUMDIGPRNT * 2;
            for(int j = 0; j <  mat1.NUM_CLM; j++){
                printf("%*d ", NUMDIGPRNT, mat1.data[i][j]);
            }
        }

        if(i == (int)(mat1.NUM_ROW / 2)){
            printf(" x ");
            space_offset -= 3;
        }

        if(i < mat2.NUM_ROW){
            for(int j = 0; j <  mat2.NUM_CLM; j++){
                if(j == 0)
                    printf("%*d ", space_offset, mat2.data[i][j]);
                else
                    printf("%*d ", NUMDIGPRNT, mat2.data[i][j]);
            }
        }

        space_offset = NUMDIGPRNTANS * 2;
        if(i == (int)(mat1.NUM_ROW / 2)){
            printf(" = ");
            space_offset -= 3;
        }

        //Print Answer
        if(i < answer.NUM_ROW){
            for(int j = 0; j <  answer.NUM_CLM; j++){
                if(j == 0)
                    printf("%*d ", space_offset, answer.data[i][j]);
                else
                    printf("%*d ", NUMDIGPRNTANS, answer.data[i][j]);
            }
        }
        printf("\n");
    }

    return answer;
}