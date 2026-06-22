#include "util.h"

void reverseArray(char * arr, int count) {
	for(int i=0; i<(count/2); i++)
		swapChar(&arr[i], &arr[count-i-1]);
}

void swapChar(char * a, char * b) {
	char temp = *a;
	*a = *b;
	*b = temp;
}
