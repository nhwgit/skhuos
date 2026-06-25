#include "Type.h"
#include "Print.h"
#include "arch/Descriptor.h"
#include "arch/RegControl.h"
#include "arch/Handler.h"
#include "mm/Memory.h"
#include "drivers/Keyboard.h"
#include "lib/Queue.h"
#include "proc/Process.h"
#include "drivers/Disk.h"
#include "drivers/PIC.h"
#include "drivers/PIT.h"
#include "fs/FAT.h"
#include "shell/Shell.h"

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
