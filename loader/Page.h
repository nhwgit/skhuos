#ifndef __PAGE_H___
#define __PAGE_H___

#include "Paging.h"

//size
#define TABLE_COUNT 512
#define PAGE_TABLE_SIZE 0x1000
#define MAX_MEMORY_SIZE 4 // 단위 : gb
#define MAX_CODE_SIZE 2 // 단위 : gb
#define KERNEL_SIZE 32 // 단위 : mb

void InitializePaging(void);
void InitializePML4Table(void);
void InitializePageDirectoryPointerTable(void);
void InitializePageDirectoryTable(void);
void InitializePageTable(void);

#endif
