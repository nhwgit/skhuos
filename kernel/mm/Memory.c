#include "mm/Memory.h"
#include "Print.h"
#include "Type.h"

static int bestIdx;
static int usingMemory;
static char memory[0x3][0x200][0x20];

static int totalMemory;

static const char * totalMemSizeMessage = "total memory size: ";
static const char * usingMemoryRateMessage = "using : ";

void initMemoryBitmap(void) {
	for(int pdt=0; pdt<TOTAL_MEMORY / 0x20000; pdt++) {
		for(int pt=0; pt<(TOTAL_MEMORY % 0x20000)/0x100; pt++) {
			for(int pm=0; pm<0x20; pm++)
				memory[pdt][pt][pm] = 0x00; // 전체 0으로 초기화
		}
	}

	for(int pt=0; pt<KERNEL_SIZE/0x100; pt++) {
		for(int pm=0; pm<0x20; pm++)
			memory[0][pt][pm] = 0xFF; // 커널부분 1로 초기화
	}
	bestIdx = 0x400;
	usingMemory = 0x4000;
}

void printMemoryRate(void) {
	int totalMemory = TOTAL_MEMORY / 0x20000;
	int usingRate = 100 * usingMemory / TOTAL_MEMORY;
	printString(totalMemSizeMessage);
	printInt(totalMemory, 10);
	printString("GB");
	puts("");
	printString(usingMemoryRateMessage);
	printInt(usingRate, 10);
	printString("%");
}

void memorySizeCheck(void) {
	DWORD * currentAddress = (DWORD *)0x4000000;
	DWORD previousValue = *currentAddress;
	while(1){
		previousValue = *currentAddress;
		*currentAddress = 0xFFFFFFFF;
		if(*currentAddress != 0xFFFFFFFF) break;
		*currentAddress = previousValue;
		currentAddress += (0x100000 / sizeof(DWORD));
	}
	totalMemory = (int)(QWORD)currentAddress / 0x100000; // 단위: MB
}

int getTotalMemory(void) {
	return totalMemory ;
}
