#include "util.h"

void reverseArray(char * arr, int count) {
	int temp = 0;
	for(int i=0; i<(count/2); i++)
		swapChar(&arr[i], &arr[count-i-1]);
}

void swapChar(char * a, char * b) {
	char temp = 0;
	temp = *a;
	*a = *b;
	*b = temp;
}

int cmp(void *a, void *b) {
	return *(int *)a - *(int *)b;
}
