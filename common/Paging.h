#ifndef __PAGING_H___
#define __PAGING_H___

#include "Type.h"

//address
#define PML4_BASE_ADDRESS 0x100000
#define PDPTABLE_BASE_ADDRESS 0x101000
#define PDTABLE_BASE_ADDRESS 0x102000
#define PTABLE_BASE_ADDRESS 0x106000

//flags
#define PAGE_LOWER4B_FLAGS_P         0x00000001
#define PAGE_LOWER4B_FLAGS_RW      	 0x00000002
#define PAGE_LOWER4B_FLAGS_US      	 0x00000004
#define PAGE_LOWER4B_FLAGS_PWT     	 0x00000008
#define PAGE_LOWER4B_FLAGS_PCD     	 0x00000010
#define PAGE_LOWER4B_FLAGS_A       	 0x00000020
#define PAGE_UPPER4B_FLAGS_EXB       0x80000000

#define PDIRECTORY_LOWER4BFLAGS_D 	 0x00000040
#define PDIRECTORY_LOWER4B_FLAGS_PS  0x00000080
#define PDIRECTORY_LOWER4B_FLAGS_G 	 0x00000100
#define PDIRECTORY_LOWER4B_FLAGS_PAT 0x00001000

#pragma pack(push, 1)
typedef struct pageEntry
{
    DWORD lower4Byte;
    DWORD upper4Byte;
} Pml4Entry, PdpEntry, PdEntry, PtEntry;
#pragma pack(pop)

#endif
