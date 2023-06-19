#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    printf("Demo Code: exec\n");

    /* Define an environment with a new home directory and a new username */
    char *env[] = { "HOME=/usr/home", "LOGNAME=home", (char *)0 };

    /* (a) Demo execl */
    printf("\nDemo #1: execl\n");
    if(fork() == 0){
        /*execl(path to exe, name of exe, argument 1, null char */
        int ret = execl("/bin/echo", "echo", "I am execl", (char*)NULL);
        perror("Error exec failed\n");
        exit(-1);
    }
    wait(NULL);

    /* (b) Demo execle */
    printf("\nDemo #2: execle\n");
    if(fork() == 0){

        /*execle(path to exe, name of exe, argument 1, null char, environment)*/
        int ret = execle("/bin/echo", "echo", "I am execle", (char*)NULL, env);
        perror("Error exec failed\n");
        exit(-1);
    }
    wait(NULL);

    /* (c) Demo execlp */
    printf("\nDemo #3: execlp\n");
    if(fork() == 0){
        /* Searches for file name on the path */
        /*execlp(file name, name of exe, argument 1, null char)*/
        int ret = execlp("echo", "echo", "I am execlp", (char*)NULL);
        perror("Error exec failed\n");
        exit(-1);
    }
    wait(NULL);

    /* (d) Demo execv */
    printf("\nDemo #4: execv\n");
    if(fork() == 0){
        /* Build array of strings with exe name, arg1 and null char */        
        char *args[] = {"echo", "I am execv", (char*)NULL};
        /*execv(path to exe, argument array)*/
        int ret = execv("/bin/echo", args);
        perror("Error exec failed\n");
        exit(-1);
    }
    wait(NULL);

    /* (e) Demo execve */
    printf("\nDemo #5: execve\n");
    if(fork() == 0){
        /* Build array of strings with exe name, arg1 and null char */        
        char *args[] = {"echo", "I am execve", (char*)NULL};
        /*execv(path to exe, argument array, env)*/
        int ret = execve("/bin/echo", args, env);
        perror("Error exec failed\n");
        exit(-1);
    }
    wait(NULL);

    /* (e) Demo execvp */
    printf("\nDemo #6: execvp\n");
    if(fork() == 0){
        /* Search for file on path */
        /* Build array of strings with exe name, arg1 and null char */        
        char *args[] = {"echo", "I am execvp", (char*)NULL};
        /*execvp(path to exe, argument array)*/
        int ret = execvp("echo", args);
        perror("Error exec failed\n");
        exit(-1);
    }
    wait(NULL);

    /* (f) Demo fexecve */
    printf("\nDemo #7: fexecve\n");
    int fd = open("/bin/echo", O_RDONLY);       /* Open a FD to the executable */
    if(fork() == 0){
        char *args[] = {"echo", "I am fexecve", (char*)NULL};
        /*fexecve(path to exe, argument array)*/
        int ret = fexecve(fd, args, env);
        perror("Error exec failed\n");
        exit(-1);
    }
    wait(NULL);
    close(fd);


    return 0;
}




