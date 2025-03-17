#ifndef __PIC_H__
#define __PIC_H__

//state
#define INITIAL 0x00
#define ICW_SET 0x01


#define MASTER_PORT1 0x20
#define MASTER_PORT2 0x21
#define SLAVE_PORT1 0xA0
#define SLAVE_PORT2 0xA1

#define IRQ_START_OFFSET 0x20
#define IRQ_MAX_COUNT 0x08

#define ICW1_DEFAULT 0x10
#define ICW1_LTIM 0x08
#define iCW1_SNGL 0x02
#define ICW1_IC4 0x01

#define ICW3_MASTER_S7 0x80
#define ICW3_MASTER_S6 0x40
#define ICW3_MASTER_S5 0x20
#define ICW3_MASTER_S4 0x10
#define ICW3_MASTER_S3 0x08
#define ICW3_MASTER_S2 0x04
#define ICW3_MASTER_S1 0x02
#define ICW3_MASTER_S0 0x01

#define ICW3_SLAVE_ID2 0x04
#define ICW3_SLAVE_ID1 0x02
#define ICW3_SLAVE_ID0 0x01

#define ICW4_SFNM 0x10
#define ICW4_BUF 0x08
#define ICW4_MS 0x04
#define ICW4_AEOI 0x02
#define ICW4_UPM 0x01

#define OCW2_R 0x80
#define OCW2_SL 0x40
#define OCW2_EOI 0x20
#define OCW2_L2 0x04
#define OCW2_L1 0x02
#define OCW2_L0 0x01

void initializePIC(void);
int sendEOI(int pin);
int setIMR(WORD mask);

#endif
