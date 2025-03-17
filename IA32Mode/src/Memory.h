#ifndef __MEMORY_H__
#define __MEMORY_H__
#include "Type.h"

#define PTABLE_BASE_ADDRESS 0x106000
#define TOTAL_MEMORY 0x60000
#define KERNEL_SIZE 0x4000
#define PT_ENTRY_ADDRESS 0x106000

#define PAGE_LOWER4B_FLAGS_P         0x00000001
#define PAGE_LOWER4B_FLAGS_RW      	 0x00000002
#define PAGE_LOWER4B_FLAGS_US      	 0x00000004
#define PAGE_LOWER4B_FLAGS_PWT     	 0x00000008
#define PAGE_LOWER4B_FLAGS_PCD     	 0x00000010
#define PAGE_LOWER4B_FLAGS_A       	 0x00000020
#define PAGE_UPPER4B_FLAGS_EXB       0x80000000

static int bestIdx;
static int usingMemory;
static char memory[0x3][0x200][0x20];

typedef struct pageEntry
{
    DWORD lower4Byte;
    DWORD upper4Byte;
} Pml4Entry, PdpEntry, PdEntry, PtEntry;

void initMemoryBitmap(void);
void AllocMemory(void);
void memcpy(void * dest, const void * src, int size);
int memcmp(const void * mem1, const void * mem2, int size);
void memsetZero(const void * mem, int size);
void memorySizeCheck(void);
int getTotalMemory(void);
#endif
