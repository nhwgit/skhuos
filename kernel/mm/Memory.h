#ifndef __MEMORY_H__
#define __MEMORY_H__
#include "Type.h"
#include "Paging.h"

#define TOTAL_MEMORY 0x60000
#define KERNEL_SIZE 0x4000

void initMemoryBitmap(void);
void printMemoryRate(void);
void memorySizeCheck(void);
int getTotalMemory(void);
#endif
