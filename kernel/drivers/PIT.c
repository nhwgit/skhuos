#include "drivers/PIT.h"
#include "arch/portControl.h"

int usToClock(QWORD us) {
	return (FREQUENCY * us) / 1000000;
}

int msToClock(QWORD ms) {
	return (FREQUENCY * ms) / 1000;
}

static int counter0Reload = 0x10000; // initializePIT 전에는 BIOS 기본 리로드 값

void initializePIT(int clock, bool periodic) {
	if(periodic)
		setPort(CTRL_REG, PIT_BCD_FALSE | PIT_MODE_2 | PIT_RW_LOWERTOUPPER | PIT_CONTROL_COUNTER0 );
	else
		setPort(CTRL_REG, PIT_BCD_FALSE | PIT_MODE_0 | PIT_RW_LOWERTOUPPER | PIT_CONTROL_COUNTER0 );
	setPort(COUNTER0, clock);
	setPort(COUNTER0, clock >> 8);
	counter0Reload = (clock & 0xFFFF) ? (clock & 0xFFFF) : 0x10000; // 카운터 값 0은 하드웨어상 0x10000
}

WORD readCounter0(void) {
	setPort(CTRL_REG, PIT_RW_LATCH | PIT_CONTROL_COUNTER0);
	BYTE lowerByte = getPort(COUNTER0);
	BYTE upperByte = getPort(COUNTER0);
	return lowerByte | (upperByte << 8);
}

void startStopwatch(PITStopwatch * watch) {
	watch->lastCounter = readCounter0();
	watch->elapsedClock = 0;
}

// 리로드 주기당 1회 이상 호출해야 랩어라운드를 놓치지 않음
int getElapsedClock(PITStopwatch * watch) {
	WORD now = readCounter0();
	int delta = watch->lastCounter - now; // 다운 카운터라 정상 진행 시 양수
	if(delta < 0)
		delta += counter0Reload;
	watch->lastCounter = now;
	watch->elapsedClock += delta;
	return watch->elapsedClock;
}
