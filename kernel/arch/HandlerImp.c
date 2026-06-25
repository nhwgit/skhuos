#include "Print.h"
#include "drivers/Keyboard.h"
#include "Type.h"
#include "drivers/PIC.h"
#include "proc/Process.h"
#include "drivers/Disk.h"
#include "arch/HandlerImp.h"

void testHandler(int vectorNumber) {
	puts("exception occur!");
	printInt(vectorNumber, 10);
}

void timerHandler(int vectorNumber) {
	sendEOI(vectorNumber - IRQ_START_OFFSET);
	timeoutSchedule();
}

void keyboardHandler(int vectorNumber) {
	if(isOutputBufferFull()) {
		BYTE scanCode = getScanCode();
		inputQueue(scanCode);
	}
	sendEOI(vectorNumber - IRQ_START_OFFSET);
}

void disk1Handler(int vectorNumber) {
	setDiskInterruptFlag(TRUE, FALSE);
	sendEOI(vectorNumber - IRQ_START_OFFSET);
}

void disk2Handler(int vectorNumber) {
	setDiskInterruptFlag(FALSE, TRUE);
	sendEOI(vectorNumber - IRQ_START_OFFSET);
}
