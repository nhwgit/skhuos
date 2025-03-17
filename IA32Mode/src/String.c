#include "String.h"

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
	int i = 0;
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
