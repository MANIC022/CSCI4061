#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 1024

#define STDIN 0
#define STDOUT 1
#define STDERR 2

char* get_line_low_level(int FD_TO_READ, int* num_char_read)
{
	//Allocate space for a string that we will later return (initial size is 1 char)
	char* line_in = malloc(sizeof(char) * 1);

	//Pre-Loop Declarations
	char* 	temp;
	char 	single_char[1];
	*num_char_read = 0;

	//Read 1 char at a time from FD_TO_READ into single_char
	
	// while(/*TODO - Read from the FD passed in one char at a time, check for error*/) 
	while(read(FD_TO_READ, single_char, 1) > 0) 
	{
		//Increment the number of charachters we have read
		(*num_char_read)++;

		//If not a newline char, allocate more space for the new char and append it to the string
		if( (temp = realloc(line_in, *num_char_read + 1)) != NULL)
		{
			line_in = temp;
			line_in[*num_char_read - 1] = *single_char;
			line_in[*num_char_read] = '\0';

			//If the char read was a newline char, break and return the string
			if(*single_char == '\n'){
				break;
			}
		}
		else
		{
			printf("Realloc Failed\n");
		}
	}

	return line_in;
}

int  main(int argc, char *argv[])
{
	int inputFd = 0;
	int outputFd = 0;
	int openFlags = 0;

	mode_t filePerms;

	openFlags = O_CREAT | O_RDWR | O_TRUNC; //Write-output
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;      /* rw-rw-rw- */
 
	if (argc != 3) 
	{
		printf("Usage: %s <source> <destination>\n", argv[0]);
		exit(1);
	}

	// 1. open source file -> argv[1] : open()

	/*TODO - Open Source File*/
	int fd_in = open(argv[1], O_RDONLY);

	// 2. open destination file -> argv[2] : open()
	
	/*TODO - Open Destination File*/
	int fd_out = open(argv[2], openFlags, filePerms);
	// int fd_out = open(argv[2], openFlags);

	// 3. Read the first 3 lines from source and write it to destination AND print to stdout 
	for(int i = 0; i < 3; i++){
		int num_char_read = 0;
		// char* line_read = get_line_low_level(/* TODO - what to pass in here?*/, &num_char_read);
		char* line_read = get_line_low_level(fd_in, &num_char_read);

		/*TODO - Write to stdout*/
		write(STDOUT_FILENO, line_read, sizeof(char) * num_char_read);

		/*TODO - Write to destination*/
		write(fd_out, line_read, sizeof(char) * num_char_read);

		free(line_read);
	}

	// 4. Read a single line from stdin and write it to destination
	printf("Please type something and hit enter:\n");
	
	int num_char_read = 0;
	// char* line_from_stdin = get_line_low_level(/* TODO - what to pass in here?*/, &num_char_read);
	char* line_from_stdin = get_line_low_level(STDIN_FILENO, &num_char_read);

	/*TODO - Write to destination*/
	write(fd_out, line_from_stdin, sizeof(char) * num_char_read);

	free(line_from_stdin);
	
	// 5. close source file : close()
	/*TODO - Close Source File*/
	close(fd_in);

	// 6. close destination file : close()
	/*TODO - Close Source File*/
	close(fd_out);

}
