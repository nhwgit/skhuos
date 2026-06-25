#include "32bit.h"
#include "Print.h"

int InitializeMemory(int start, int end) {
	int * address = (int *)start;
	while((int)address<end) {
		*address = 0;
		if(*address!=0)
			return printStateAndReturn(FALSE);
		address++;
	}
	return printStateAndReturn(TRUE);
}

void copyIA32ModeImage(void) {
	short totalKernelSectorCount = *((short*)(0x00007C05));
	short kernel32SectorCount = *((short*)(0x00007C07));
	int * src = (int *)(PROTECTED_START_ADDRESS+(kernel32SectorCount * 512));
	int * dest = (int *)IA32_START_ADDRESS;
	for(int i=0; i<512 *(totalKernelSectorCount - kernel32SectorCount) /4; i++) {
		*dest = *src;
		dest++;
		src++;
	}
}
