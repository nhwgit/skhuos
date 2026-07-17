#include "Print.h"
#include "Type.h"
#include "arch/HandlerImp.h"
#include "arch/CPU.h"
#include "arch/RegControl.h"
#include "arch/Descriptor.h"
#include "proc/Process.h"

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

static InterruptHandler interruptHandlers[INTERRUPT_VECTOR_COUNT];

void registerInterruptHandler(int vectorNumber, InterruptHandler handler) {
	if(vectorNumber<0 || vectorNumber>=INTERRUPT_VECTOR_COUNT)
		return;
	interruptHandlers[vectorNumber] = handler;
}

void dispatchInterrupt(int vectorNumber) {
	InterruptHandler handler = interruptHandlers[vectorNumber];
	if(handler==NULL) {
		puts("unhandled interrupt!");
		printInt(vectorNumber, 10);
		return;
	}
	handler(vectorNumber);
}

void exceptionHandler(int vectorNumber, QWORD errorCode, QWORD rip, QWORD cs) {
	disableInterrupt();
	if((cs & RPL_USER) == RPL_USER) { // 링3 예외는 시스템 정지 대신 해당 프로세스만 종료
		moveToNextLine();
		printString("Ring3 process(pid ");
		printInt(getRunningPid(), 10);
		printString(") killed: ");
		printString(exceptionNames[vectorNumber]);
		printString(" RIP: 0x");
		printHex(rip);
		moveToNextLine();
		exitProcess(); // 복귀 없음 — 다음 프로세스로 전환
	}
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
