#ifndef __HANDLER_IMP_H__
#define __HANDLER_IMP_H__

#define IRQ_TIMER 		0
#define IRQ_KEYBOARD 	1
#define IRQ_SLAVE_PIC 	2
#define IRQ_COM2		3
#define IRQ_COM3		4
#define IRQ_PRINT2		5
#define IRQ_FLOPPY		6
#define IRQ_PRINT1		7

#define IRQ_RTC			8
#define IRQ_REVERSED	9
#define IRQ_MOUSE		12
#define IRQ_COPROCESSOR	13
#define IRQ_DISK1		14
#define IRQ_DISK2		15

void testHandler(int vectorNumber);
void keyboardHandler(int vectorNumber);
void timerHandler(int vectorNumber);

#endif
