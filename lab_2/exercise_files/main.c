#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){

     
//  1. Takes an integer argument from the command line and store it in a variable. example. "num".
     // if argument is not provided, print error message
     
// 2. Forks two children processes
     // a. First child computes and prints the first Nth terms of Fibonacci Sequence.  
     // and prints out the result and its own identifier ID (process id).
     // Example:   [ ID = 262750] The list of first 5 terms in the Fibonacci Sequence is: 0, 1, 1, 2, 3
     
     
     // b. Second child computes the factorial of Number (1*2*...*N1) and prints out
     // the result and its own identifier ID.
     // Example [ ID = 4076743] Factorial of 5 is 120
     
// 3. Parent waits until both children are finished, then prints out the message “Done”
     // and its own identifier.
     // Example: [ID = 4076741] DONE
}
