#include "Print.h"
#include "drivers/Keyboard.h"
#include "Type.h"
#include "drivers/PIC.h"
#include "proc/Process.h"
#include "drivers/Disk.h"
#include "arch/HandlerImp.h"
#include "arch/RegControl.h"

static const char * exceptionNames[] = {
	"#DE Divide Error",
	"#DB Debug",
	"NMI",
	"#BP Breakpoint",
	"#OF Overflow",
	"#BR Bound Range Exceeded",
	"#UD Invalid Opcode",
	"#NM Device Not Available",
	"#DF Double Fault",
	"Coprocessor Segment Overrun",
	"#TS Invalid TSS",
	"#NP Segment Not Present",
	"#SS Stack Segment Fault",
	"#GP General Protection",
	"#PF Page Fault",
	"Reserved",
	"#MF FPU Error",
	"#AC Alignment Check",
	"#MC Machine Check",
	"#XM SIMD Error",
	"Reserved", // 벡터 21 이상도 IV20으로 수렴하므로 마지막 엔트리가 받음
};

void exceptionHandler(int vectorNumber, QWORD errorCode, QWORD rip) {
	disableInterrupt();
	moveToNextLine();
	printString("==== Exception: ");
	printString(exceptionNames[vectorNumber]);
	printString(" (vector ");
	printInt(vectorNumber, 10);
	puts(") ====");
	printString("RIP: 0x");
	printHex(rip);
	moveToNextLine();
	printString("Error Code: 0x");
	printHex(errorCode);
	moveToNextLine();
	if(vectorNumber==PAGE_FAULT_VECTOR) {
		printString("CR2: 0x");
		printHex(getCR2());
		moveToNextLine();
	}
	puts("System halted");
	while(TRUE)
		halt();
}

void testHandler(int vectorNumber) {
	puts("unhandled interrupt!");
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
