#ifndef __PIT_H__
#define __PIT_H__

#include "type.h"

#define COUNTER0	0x40
#define COUNTER1	0x41
#define COUNTER2	0x42
#define CTRL_REG	0x43

#define FREQUENCY	1193182

#define PIT_BCD_TRUE 0x01
#define PIT_BCD_FALSE 0x00

#define PIT_MODE_0	0x00
#define PIT_MODE_1	0x02
#define PIT_MODE_2	0x04
#define PIT_MODE_3	0x06
#define PIT_MODE_4	0x08
#define PIT_MODE_5	0x0A

#define PIT_RW_LATCH	0x00
#define PIT_RW_LOWER	0x10
#define PIT_RW_UPPER	0x20
#define PIT_RW_LOWERTOUPPER	0x30

#define PIT_CONTROL_COUNTER0 0x00
#define PIT_CONTROL_COUNTER1 0x40
#define PIT_CONTROL_COUNTER2 0x80

void sleep(long us);
int usToClock(QWORD us);
int msToClock(QWORD ms);
void initilizePIT(int clock, bool periodic);
int timeToClock(WORD us);
WORD readCounter0(void);

#endif
