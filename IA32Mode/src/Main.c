#include "Type.h"
#include "Print.h"
#include "Descriptor.h"
#include "RegControl.h"
#include "Handler.h"
#include "Memory.h"
#include "Keyboard.h"
#include "Queue.h"
#include "Process.h"
#include "Disk.h"
#include "PIC.h"
#include "PIT.h"
#include "FAT.h"
#include "Shell.h"

void Main(void) {
	cursorLine(7); // 부팅 메시지 다음 줄부터 출력
	puts("Switch to IA-32e mode");
	puts("Success Booting");

	initializeDescriptor();
	puts("Initialize Descriptor");

	loadGDTR(GDTR_ADDRESS);
	loadTR(TSS_DESCRIPTOR_OFFSET);
	loadIDTR(IDTR_ADDRESS);
	puts("Descriptor Load Complete");

	initializePIC();
	setIMR(0);
	enableInterrupt();
	puts("PIC initialize");
	initKeyboard();
	initMemoryBitmap();
	puts("Memory Pool Initialize");
	initScheduler();
	initializePIT(msToClock(SLOT_TIME), 1);
	initDisk();
	puts("Disk Initialize");
	initFAT();
	startShell();
	while(1);
}
