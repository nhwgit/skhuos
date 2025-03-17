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

const char * switchToIA32eMode = "Switch to IA-32e mode";
const char * successBooting = "Success Booting";
const char * initDiscriptor = "Initilize Discriptor";
const char * discriptorLoadComplete = "Discriptor Load Complete";
const char * picInitilize = "PIC initilize";
const char * intInitilize = "Interrupt Initilize";
const char * memPoolInitlize = "Memory Pool Initilize";
const char * diskInitlize = "Disk Initilize";
//const char * memSizeCheck = "Memory Size Check Complete";

void Main(void) {
	int a=1;
	puts(switchToIA32eMode);
	puts(successBooting);

	initilizeDiscriptor();
	puts(initDiscriptor);

	loadGDTR(GDTR_ADDRESS);
	loadTR(TSS_DESCRIPTOR_OFFSET);
	loadIDTR(IDTR_ADDRESS);
	puts(discriptorLoadComplete);

	initializePIC();
	setIMR(0);
	enableInterrupt();
	puts(picInitilize);
	initKeyboard();
	initMemoryBitmap();
	puts(memPoolInitlize);
	initScheduler();
	initilizePIT(msToClock(SLOT_TIME), 1);
	initDisk();
	puts(diskInitlize);
	initFAT();
	startShell();
	while(1);
}
