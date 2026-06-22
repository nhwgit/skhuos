#include "String.h"
#include "Type.h"

int strlen(const char* str) {
	int idx=0;
	while(1) {
		if(str[idx]==0)
			break;
		idx++;
	}
	return idx;
}

int strcmp(const char * str1, const char * str2) {
	int tmp = 0;
	for(int i=0; ; i++) {
		tmp = str1[i]-str2[i];
		if(tmp!=0)
			return tmp;
		if(str1[i]==0 || str2[i] ==0)
			return tmp;
	}
	return 0;
}

void memcpy(void * dest, const void * src, int size) {
	for(int i=0; i<size; i++)
		((BYTE*)dest)[i] = ((BYTE*)src)[i];
}

int memcmp(const void * mem1, const void * mem2, int size) {
	BYTE tmp;
	for(int i=0; i<size; i++) {
		tmp = ((BYTE*)mem1)[i]-((BYTE*)mem2)[i];
		if(tmp!=0)
			return tmp;
	}
	return 0;
}

void memsetZero(void * mem, int size) {
	for(int i=0; i<size; i++) {
		((BYTE*)mem)[i] = 0;
	}
}
