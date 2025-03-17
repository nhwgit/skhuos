#include "Memory.h"
#include "Print.h"
#include "Type.h"

static int bestIdx;
static char memory[0x3][0x200][0x20];

static int totalMemory;

const char * totalMemSizeMessage = "total memory size: ";
const char * usingMemoryRateMessage = "using : ";

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

/*int AllocMemory(void) { // kb 단위
	int pdt=0, pt=0, pm=0;
	pdt = bestIdx % 0x100;
	pt = (bestIdx % 0x20000) / 0x100;
	pm = bestIdx / 0x20000;
	for(int i=pdt; i<0x3; i++) {
		for(int j=pt; j<0x200; j++) {
			for(int k=pm; k<0x20; k++) {
				for(int bitOffset = 0; bitOffset<8; i++){
					if((memory[i][j][k] << (7-i)) & 1 == 0)

				}
			}
		}
	}
}*/

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

void memcpy(void * dest, const void * src, int size) {
	for(int i=0; i<size; i++)
		((BYTE*)dest)[i] = ((BYTE*)src)[i];
}

int memcmp(const void * mem1, const void * mem2, int size) {
	BYTE tmp;
	for(int i=0; i<size; i++) {
		tmp = ((BYTE*)mem1)[i]-((BYTE*)mem2)[i];
		if(tmp!=0)
			return tmp;
	}
	return 0;
}

void memsetZero(const void * mem, int size) {
	BYTE tmp;
	for(int i=0; i<size; i++) {
		((BYTE*)mem)[i] = 0;
	}
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
	totalMemory = totalMemory / 0x100000;
}

int getTotalMemory(void) {
	return totalMemory ;
}
