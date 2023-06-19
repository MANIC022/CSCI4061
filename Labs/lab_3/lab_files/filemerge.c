#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define MAXBUFFER 128

#define max(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

int stringCompare(char str1[], char str2[]) {
	/**
	 * returns a value greater than 0 if str1 > str2
	 * and 0 if str1 == str2
	 */
	
	int i, str1_len = strlen(str1), str2_len = strlen(str2);
	
	for (i=0; i<max(str1_len,str2_len); i++) {
		int _val;
		
		if ((_val = strncmp(str1+i,str2+i,1)) != 0) 
			return _val;
	}
	
	if (str1_len == str2_len) {
		return 0;
	} else if (str1_len > str2_len) {
		return 1;
	} else {
		return -1;
	}
	
}

int stringMerge(char filename1[], char filename2[] , char filename3[]) {
	
	FILE *fd1, *fd2, *fd3;
	char buffer1[MAXBUFFER];
	char buffer2[MAXBUFFER];
	int n=0;
	
	// 1. open all three files
	// 2. read a line from each input file
	// 3. compare each line using stringCompare
	// 4. print the lines into filename3 in alphebetical order
	// 5. return the number of lines copied into filename3
	
	return n;
}

int main(int argc, char *argv[]) {
	
	if(argc!=4)
	{
		printf("Usage: %s sortedfile1 sortedfile2 mergefile\n", argv[0]);
		exit(0);
	}
	printf("We have merged %d records\n", stringMerge(argv[1], argv[2], argv[3]));
}


