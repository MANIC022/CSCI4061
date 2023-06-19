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
	while(read(FD_TO_READ, single_char, sizeof(single_char))>0) 
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
	if((inputFd = open(argv[1], filePerms)) == -1)
	{
		perror("Failed to open input file.");
		return -1;
	}

	// 2. open destination file -> argv[2] : open()
	
	/*TODO - Open Destination File*/
	if((outputFd = open(argv[2], openFlags, filePerms)) == -1)
	{
		perror("Failed to open output file.");
		return -1;
	}


	// 3. Read the first 3 lines from source and write it to destination AND print to stdout 
	for(int i = 0; i < 3; i++){
		int num_char_read = 0;
		char* line_read = get_line_low_level(inputFd, &num_char_read);

		/*TODO - Write to stdout*/
		printf("Line Read: %s", line_read);

		/*TODO - Write to destination*/
		int written = write(outputFd, line_read, sizeof(char)  * num_char_read);
		
		if (written == -1) {
			printf("Write Error\n");
		}

		free(line_read);
	}

	// 4. Read a single line from stdin and write it to destination
	printf("Please type something and hit enter:\n");
	/*TODO - Write to destination*/
	int num_char_read = 0;
	char* line_from_stdin = get_line_low_level(STDIN, &num_char_read);
	int written_stdin = write(outputFd, line_from_stdin, sizeof(char) * num_char_read);
	if (written_stdin == -1) {
		printf("Write Error\n");
	}	

	free(line_from_stdin);
	
	// 5. close source file : close()
	/*TODO - Close Source File*/
	int close_fd = close(inputFd);
	if(close_fd == -1){
		printf("Close Error for input\n");
	}

	// 6. close destination file : close()
	/*TODO - Close Source File*/
	close_fd = close(outputFd);
	if(close_fd == -1){
		printf("Close Error for output\n");
	}

}
