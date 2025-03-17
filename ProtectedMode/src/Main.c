#include "32bit.h"
#include "Page.h"
#include "Print.h"
#include "SwitchToIA32.h"
#include "Type.h"

const char * initializeKernelMessage = "Page table & Kernel area initialize...";
const char * initializePageTableMessage = "page table initialize";

void Main(void) {
	printString(initializeKernelMessage);
	if(!InitializeMemory(PAGE_TABLE_START_ADDRESS, IA32_END_ADDRESS))
		while(1);
	printString(initializePageTableMessage);
	InitializePageTable();
	copyIA32ModeImage();
	SwitchIA32Mode();
}
