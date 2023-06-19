#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 

int main(int argc, char **argv)
{
	int in, out;
	char *grep_args[] = {"grep", "find", NULL};

	// open input and output files

	//open file "input" with read-only permission
	if((in = open("input", O_RDONLY)) == -1)
	{
		perror("Failed to open input.");
		return -1;
	}

	//open file "dup2_output", 
	//file permission flags: S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR (rw-rw-rw) / use ls to check
	//open mode of file: O_WRONLY | O_TRUNC | O_CREAT - use all perms
	if((out = open("dup2_output", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) == -1)
	{
		perror("Failed to open dup2_output.");
		return -1;
	}


	if (dup2(in, STDIN_FILENO) == -1) //stdin fd
	{
		perror("Failed to redirect stdin.");
	    return -1;
	}

	if (dup2(out, STDOUT_FILENO) == -1) //stdout fd
	{
		perror("Failed to redirect stdout.");
		return -1;
	}

	// close unused file descriptors
	close(in);
	close(out);

	// execute grep
	//grep will perform operations on "in" and "out" not stdin/out
	execvp("grep", grep_args);
}