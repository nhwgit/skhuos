#include "64bit.h"
#include "Type.h"

int InitializeMemory(int start, int end) {
	int * address = (int *)start;
	while((int)address<end) {
		*address = 0;
		if(*address!=0)
			return FALSE;
		address++;
	}
	return TRUE;
}
