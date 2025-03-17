#include "PIT.h"
#include "portControl.h"
#include "Print.h"

int usToClock(QWORD us) {
	return (FREQUENCY * us) / 1000000;
}

int msToClock(QWORD ms) {
	return (FREQUENCY * ms) / 1000;
}

void initilizePIT(int clock, bool periodic) {
	if(periodic)
		setPort(CTRL_REG, PIT_BCD_FALSE | PIT_MODE_2 | PIT_RW_LOWERTOUPPER | PIT_CONTROL_COUNTER0 );
	else
		setPort(CTRL_REG, PIT_BCD_FALSE | PIT_MODE_0 | PIT_RW_LOWERTOUPPER | PIT_CONTROL_COUNTER0 );
	setPort(COUNTER0, clock);
	setPort(COUNTER0, clock >> 8);
}

WORD readCounter0(void) {
	setPort(CTRL_REG, PIT_RW_LATCH | PIT_CONTROL_COUNTER0);
	BYTE lowerByte = getPort(COUNTER0);
	BYTE upperByte = getPort(COUNTER0);
	return lowerByte | (upperByte << 8);
}

void sleep(long us) {
	initilizePIT(usToClock(0x0), TRUE);
	long remainingTime = us;
	WORD time1 = readCounter0();
	while(remainingTime>0) {
		WORD time2 = readCounter0();
		remainingTime -= ((time1-time2)&0xFFFF);
		time1 = time2;
	}
}
