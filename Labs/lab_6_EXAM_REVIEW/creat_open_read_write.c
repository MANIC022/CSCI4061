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
    printf("Demo Code: creat + open + read + write + close \n");

    /* Define file permissions */
    mode_t file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    /* (a) Create a file with creat */
    int c_fd = creat("creat_output", file_perms);
    if(c_fd == -1){
        perror("error using creat\n");
        exit(-1);
    }
    printf("(1) Success created 'creat_output'\n");

    /* (b) Open another file with open and create flags */
    int o_fd = open("open_output", O_WRONLY|O_CREAT|O_TRUNC, file_perms);
    if(o_fd == -1){
        perror("error using open (create mode)\n");
        exit(-1);
    }
    printf("(2) Success opened 'open_output' w/ creat mode\n");

    /* (c) Open this C file in read mode */
    int r_fd = open("creat_open_read_write.c", O_RDONLY);
    if(r_fd == -1){
        perror("error using open (read mode)\n");
        exit(-1);
    }
    printf("(3) Success opened 'creat_open_read_write.c' w/ read only mode\n");

    /* (d) Write to creat_output */
    char *str_1 = "Write To creat_output\n";
    int num_byte_written = write(c_fd, str_1, strlen(str_1));
    if(num_byte_written == -1){
        perror("error using write\n");
        exit(-1);
    }
    printf("(4) Success wrote [%d] bytes to 'creat_output'\n", num_byte_written);

    /* (e) Read from our C file */
    char read_str[30];
    memset(read_str, '\0', 30);
    int num_bytes_read = read(r_fd, read_str, 29);      // Leave last char as null so we can print as string
    if(num_bytes_read == -1){
        perror("error using read\n");
        exit(-1);
    }
    printf("(5) Success read [%d] bytes to 'creat_open_read_write.c'\n", num_bytes_read);

    /* (f) Write to open_output what we read from creat_open_read_write.c */
    num_byte_written = write(o_fd, read_str, 29);
    if(num_byte_written == -1){
        perror("error using write\n");
        exit(-1);
    }
    printf("(6) Success wrote [%d] bytes to 'creat_output'\n", num_byte_written);
    printf("    --> Wrote this string = '%s'\n", read_str);

    /* (g) Close all files */
    int ret_val = close(c_fd);
    if(ret_val == -1){
        perror("error using close\n");
        exit(-1);
    }
    ret_val = close(o_fd);
    if(ret_val == -1){
        perror("error using close\n");
        exit(-1);
    }
    ret_val = close(r_fd);
    if(ret_val == -1){
        perror("error using close\n");
        exit(-1);
    }

    printf("(7) Success! Closed all three files\n");

    return 0;
}




