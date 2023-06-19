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
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    printf("Demo Code: getcwd + link + symlink + unlink + stat\n");

    /* (a) Get CWD version 1 */
    char *cwd_buf = malloc(100);
    char *tmp_buf = getcwd(cwd_buf, 100);
    if(tmp_buf == NULL){
        perror("error using getcwd\n");
        exit(-1);
    }
    printf("(1) Success get the cwd [%s]\n", cwd_buf);
    printf("    --> cwd_buf[%p] tmp_buf[%p]\n", cwd_buf, tmp_buf);
    free(cwd_buf);

    /* (b) Get CWD Version 2 */
    char *cwd_buf2 = getcwd(NULL, 100);
    if(cwd_buf2 == NULL){
        perror("error using getcwd\n");
        exit(-1);
    }
    printf("(2) Success get the cwd (2) [%s]\n", cwd_buf2);
    free(cwd_buf2);

    /* (c) Use Link */
    int ret_val = link("misc_sys_calls.c", "link_check");
    if(ret_val == -1){
        perror("error using link\n");
        exit(-1);
    }
    printf("(3) Success created a hard link from 'misc_sys_calls.c' to 'link_check'\n");

    /* (d) Use unlink */
    ret_val = unlink("link_check");
    if(ret_val == -1){
        perror("error using unlink\n");
        exit(-1);
    }
    printf("(4) Success unlinked 'link_check'\n");

    /* (e) use symlink */
    ret_val = symlink("misc_sys_calls.c", "link_check_sym");
    if(ret_val == -1){
        perror("error using symlink\n");
        exit(-1);
    }
    printf("(5) Success created a symbolic link from 'misc_sys_calls.c' to 'link_check_sym'\n");

    /* (f) Use Stat */
    struct stat local_stat;
    ret_val = stat("misc_sys_calls.c", &local_stat);
    if(ret_val == -1){
        perror("error using stat\n");
    }
    printf("(6) Success got the stats for 'misc_sys_calls.c'\n");
    printf("    st_dev: [%ld]\n", local_stat.st_dev);
    printf("    inode:  [%ld]\n", local_stat.st_ino);

    return 0;
}




