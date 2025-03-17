#include "portControl.h"
#include "PIC.h"
#include "Print.h"

static int state = INITIAL;
void initializePIC(void) {
	setPort(MASTER_PORT1, ICW1_DEFAULT | ICW1_IC4);
	setPort(MASTER_PORT2, IRQ_START_OFFSET);
	setPort(MASTER_PORT2, ICW3_MASTER_S2);
	setPort(MASTER_PORT2, ICW4_UPM);

	setPort(SLAVE_PORT1, ICW1_DEFAULT | ICW1_IC4);
	setPort(SLAVE_PORT2, IRQ_START_OFFSET+IRQ_MAX_COUNT);
	setPort(SLAVE_PORT2, ICW3_SLAVE_ID1);
	setPort(SLAVE_PORT2, ICW4_UPM);
	state = ICW_SET;
}

int setIMR(WORD mask) {
	if(state!=ICW_SET) return -1;
	setPort(MASTER_PORT2, mask & 0xFF);
	setPort(SLAVE_PORT2, (mask & 0xFF00) >> 8);
	return 0;
}

int sendEOI(int pin) {
	if(state!=ICW_SET) return -1;
	setPort(MASTER_PORT1, OCW2_EOI);
	if(pin>=8)
		setPort(SLAVE_PORT1, OCW2_EOI);
	return 0;
}
