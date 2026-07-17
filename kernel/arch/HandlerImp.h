#ifndef __HANDLER_IMP_H__
#define __HANDLER_IMP_H__
#include "Type.h"

#define PAGE_FAULT_VECTOR 14
#define RESERVED_VECTOR 20 // 스텁이 없는 예약 벡터(21~31, 48 이상)가 수렴하는 벡터
#define INTERRUPT_VECTOR_COUNT 48

#define IRQ_TIMER 		0
#define IRQ_KEYBOARD 	1
#define IRQ_SLAVE_PIC 	2
#define IRQ_COM2		3
#define IRQ_COM3		4
#define IRQ_PRINT2		5
#define IRQ_FLOPPY		6
#define IRQ_PRINT1		7

#define IRQ_RTC			8
#define IRQ_RESERVED	9
#define IRQ_MOUSE		12
#define IRQ_COPROCESSOR	13
#define IRQ_DISK1		14
#define IRQ_DISK2		15

typedef void (*InterruptHandler)(int vectorNumber);

void registerInterruptHandler(int vectorNumber, InterruptHandler handler);
void dispatchInterrupt(int vectorNumber);
void exceptionHandler(int vectorNumber, QWORD errorCode, QWORD rip, QWORD cs);

#endif
