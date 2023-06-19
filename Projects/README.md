/* CSCI - 4061 Fall 2022 - Project 3
 * Group Member #1: Amit Manicka <manic022>
 * Group Member #2: Aditya Prabhu <prabh079>
 * Group Member #3: Anton Priborkin <pribo002>
*/

/* Division of work:
 * Amit: main(), worker(), dispatch(), getCacheIndex(), addIntoCache(), deleteCache(), initCache(),
    getContentType(), readFromDisk(), dispatch(), worker(), some error checking of system calls and testing
 * 
 * Aditya: worker(), dispatcher(), vast majority of testing and error checking of system calls
 * 
 * Anton: minimal, brief testing
*/

/*Cache Implementation:
 * We have decided to implement a FIFO Cache, so the first entry in is first to be evicted when cache is full
*/

