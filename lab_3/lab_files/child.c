#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


/* Child process executes this code */

void main(int argc, char *argv[])
{
  printf("CHILD: I am the child process after exec\n");
  printf("CHILD: Number of args passed to me %d\n", argc);

 { int i;
   for (i=0; i<argc; i++) {
     printf("CHILD: Argument %d is %s \n", i, argv[i] );
    }
 }

}
