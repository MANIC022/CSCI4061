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
        fd1 = fopen(filename1, "r");
        fd2 = fopen(filename2, "r");
        fd3 = fopen(filename3, "w");
        if (fd1 == NULL || fd2 == NULL || fd3 == NULL) {
          perror("Error ");
          exit(EXIT_FAILURE);
        }
	// 2. read a line from each input file
        fgets(buffer1, MAXBUFFER, fd1);
        fgets(buffer2, MAXBUFFER, fd2);
        while (!feof(fd1) && !feof(fd2)) {
          if (stringCompare(buffer1, buffer2) < 0) {
            fprintf(fd3, "%s", buffer1);
            fgets(buffer1, MAXBUFFER, fd1);
            ++n;
          } else {
            fprintf(fd3, "%s", buffer2);
            fgets(buffer2, MAXBUFFER, fd2);
            ++n;
          }
        }
        while (!feof(fd1)) {
            fprintf(fd3, "%s", buffer1);
            fgets(buffer1, MAXBUFFER, fd1);
            ++n;
        }
        while (!feof(fd2)) {
            fprintf(fd3, "%s", buffer2);
            fgets(buffer2, MAXBUFFER, fd2);
            ++n;
        }
        fclose(fd1);
        fclose(fd2);
        fclose(fd3);
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


