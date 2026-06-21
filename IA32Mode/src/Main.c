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
	puts("Switch to IA-32e mode");
	puts("Success Booting");

	initilizeDiscriptor();
	puts("Initilize Discriptor");

	loadGDTR(GDTR_ADDRESS);
	loadTR(TSS_DESCRIPTOR_OFFSET);
	loadIDTR(IDTR_ADDRESS);
	puts("Discriptor Load Complete");

	initializePIC();
	setIMR(0);
	enableInterrupt();
	puts("PIC initilize");
	initKeyboard();
	initMemoryBitmap();
	puts("Memory Pool Initilize");
	initScheduler();
	initilizePIT(msToClock(SLOT_TIME), 1);
	initDisk();
	puts("Disk Initilize");
	initFAT();
	startShell();
	while(1);
}
