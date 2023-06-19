// This is a string at the top of the file

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    printf("Demo Code: fopen + fread + fwrite + fseek + fclose\n");

    /* (a) Create a file with fopen */
    FILE *op_strm = fopen("fopen_output", "w");
    if(op_strm == NULL){
        perror("error using fopen\n");
        exit(-1);
    }
    printf("(1) Success created 'fopen_output'\n");

    /* (b) Open this C file in read mode */
    FILE *rd_strm = fopen("fopen_fread_fwrite_fseek.c", "r");
    if(rd_strm == NULL){
        perror("error using fopen\n");
        exit(-1);
    }
    printf("(2) Success opened 'fopen_fread_fwrite_fseek.c' w/ read only mode\n");

    /* (c) Read from rd_strm */
    char str_in[30];
    memset(str_in, '\0', 30);
    ssize_t read_size = fread(str_in, 1, 30, rd_strm);
    if(read_size == EOF){
        perror("error calling fread\n");
        exit(-1);
    }
    printf("(3) Success read [%ld] bytes from 'fopen_fread_fwrite_fseek.c'\n", read_size);

    /* (d) Write to op_strm */
    ssize_t write_size = fwrite(str_in, 1, read_size, op_strm);
    if(write_size != read_size){
        perror("error calling fwrite\n");
        exit(-1);
    }
    printf("(4) Success wrote [%ld] bytes to 'fopen_output'\n", write_size);

    /* (e) Demonstrate the fseek function */
    int before = ftell(rd_strm);
    if(before == -1){
        perror("error using ftell\n");
        exit(-1);
    }
    int seek_ret_val = fseek(rd_strm, 0L, SEEK_SET);
    if(seek_ret_val == -1){
        perror("error using fseek\n");
        exit(-1);
    }
    int after = ftell(rd_strm);
    if(after == -1){
        perror("error using ftell\n");
        exit(-1);
    }
    printf("(5) Success used fseek to seek to the start of the file FP_before[%d] -> FP_after[%d]\n", before, after);


    /* (f) Close all files */
    int ret_val = fclose(op_strm);
    if(ret_val == EOF){
        perror("error using fclose\n");
        exit(-1);
    }
    ret_val = fclose(rd_strm);
    if(ret_val == EOF){
        perror("error using fclose\n");
        exit(-1);
    }
    printf("(6) Success, closed all files \n");



    

    return 0;
}




