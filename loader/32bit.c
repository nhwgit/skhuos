#include "32bit.h"
#include "BootInfo.h"
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

static const BootInfo * findBootInfo(void) {
	const char * bootSector = (const char *)BOOT_SECTOR_LOADED_ADDRESS;
	for(int i=0; i + (int)sizeof(BootInfo) <= BOOT_SECTOR_SIZE; i++) {
		const char * signature = BOOT_INFO_SIGNATURE;
		int match = 1;
		for(int j=0; j<BOOT_INFO_SIGNATURE_LENGTH; j++) {
			if(bootSector[i+j] != signature[j]) {
				match = 0;
				break;
			}
		}
		if(match)
			return (const BootInfo *)(bootSector + i);
	}
	return 0;
}

void copyIA32ModeImage(void) {
	const BootInfo * bootInfo = findBootInfo();
	if(bootInfo == 0) {
		printStateAndReturn(FALSE);
		while(1);
	}
	int * src = (int *)(PROTECTED_START_ADDRESS+(bootInfo->loaderSectorCount * 512));
	int * dest = (int *)IA32_START_ADDRESS;
	for(int i=0; i<512 *(bootInfo->totalSectorCount - bootInfo->loaderSectorCount) /4; i++) {
		*dest = *src;
		dest++;
		src++;
	}
}
