#ifndef __MEMORY_H__
#define __MEMORY_H__
#include "Type.h"
#include "Paging.h"

#define TOTAL_MEMORY 0x60000
#define KERNEL_SIZE 0x4000

void initMemoryBitmap(void);
void printMemoryRate(void);
void memcpy(void * dest, const void * src, int size);
int memcmp(const void * mem1, const void * mem2, int size);
void memsetZero(void * mem, int size);
void memorySizeCheck(void);
int getTotalMemory(void);
#endif
