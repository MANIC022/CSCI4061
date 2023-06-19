#include <stdio.h>

int main(int argc, char ** argv)
{
	int i=0;
	int a[8] = {1,2,3,4,5,6,7,8};
	char c[8] = {'a','b','c','d','e','f','g','h'};
	int *pa;
	char *pc;
	pa = &a[0];
	pc = &c[0];
	
	for(i=0;i<8;i++)
	{
		printf("%d. pa_add: %p val : %d, pc_add : %p val : %c\n", i, pa+i, *(pa+i), pc+i, *(pc+i));
	}
	
	return 0;
}

