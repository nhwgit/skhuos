#include "32bit.h"
#include "Page.h"
#include "Print.h"
#include "SwitchToIA32.h"
#include "Type.h"

const char * initializeKernelMessage = "Page table & Kernel area initialize...";
const char * initializePageTableMessage = "page table initialize";

void Main(void) {
	cursorLine(5); // 부팅 메시지 다음 줄부터 출력
	printString(initializeKernelMessage);
	if(!InitializeMemory(PAGE_TABLE_START_ADDRESS, IA32_END_ADDRESS))
		while(1);
	printString(initializePageTableMessage);
	InitializePaging();
	copyIA32ModeImage();
	SwitchIA32Mode();
}
