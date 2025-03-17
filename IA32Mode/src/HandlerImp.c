#include "Print.h"
#include "Keyboard.h"
#include "type.h"
#include "PIC.h"
#include "Process.h"
#include "Disk.h"
#include "HandlerImp.h"

//static int timerCount = 0;

void testHandler(int vectorNumber) {
	puts("exception occur!");
	printInt(vectorNumber, 10);
}

void interruptHandler(int vectorNumber) {
	sendEOI(vectorNumber - IRQ_START_OFFSET);
}

void timerHandler(int vectorNumber) {
	//timerCount = (timerCount+1)%10;
	//if(timerCount == 10)
	sendEOI(vectorNumber - IRQ_START_OFFSET);
	timeoutSchedule();
}

void keyboardHandler(int vectorNumber) {
	//puts("interrupt occur!");
	if(isOutputBufferFull) {
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
