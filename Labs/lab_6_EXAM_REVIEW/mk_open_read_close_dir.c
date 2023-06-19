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
#include <dirent.h>

int main(int argc, char *argv[]) {
    printf("Demo Code: mkdir + opendir + readdir + closedir\n");

    mode_t dir_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    /* (a) Make a directory */
    int ret_val = mkdir("test", dir_perms);
    if(ret_val == -1){
        perror("error using mkdir\n");
        exit(-1);
    }
    printf("(1) Success created a test directory\n");

    /* (b) Open parent directory */
    DIR *op_dir = opendir("..");
    if(op_dir == NULL){
        perror("error using opendir\n");
        exit(-1);
    }

    printf("(2) Reading Directory:\n");
    struct dirent *readdir_val;
    int read_count = 0;
    while(readdir_val = readdir(op_dir)){
        printf("Directory Entry [%d]:\n", read_count++);
        printf("    I-Node: %ld\n", readdir_val->d_ino);
        printf("    Offset: %ld\n", readdir_val->d_off);
        printf("    Length: %d\n", readdir_val->d_reclen);
        printf("    Type:   %d\n", readdir_val->d_type);
        printf("    Name:   %s\n", readdir_val->d_name);
    }

    ret_val = closedir(op_dir);
    if(ret_val == -1){
        perror("error using closedir\n");
        exit(-1);
    }
    printf("(3) Success was able to close the parent directory\n");





    

    return 0;
}




