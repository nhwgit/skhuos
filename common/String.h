#ifndef __STRING_H__
#define __STRING_H__

int strlen(const char* str);
int strcmp(const char * str1, const char * str2);
void memcpy(void * dest, const void * src, int size);
int memcmp(const void * mem1, const void * mem2, int size);
void memsetZero(void * mem, int size);
#endif
