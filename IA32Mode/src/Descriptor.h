#ifndef __DESCRIPTOR_H___
#define __DESCRIPTOR_H___

#include "Type.h"

#define GDT_FLAG_G 0x00800000
#define GDT_FLAG_DB 0x00400000
#define GDT_FLAG_L 0x00200000
#define GDT_FLAG_AVL 0x00100000
#define GDT_FLAG_P 0x00008000
#define GDT_FLAG_DPL0 0x00000000
#define GDT_FLAG_DPL1 0x00002000
#define GDT_FLAG_DPL2 0x00004000
#define GDT_FLAG_DPL3 0x00006000
#define GDT_FLAG_S 0x00001000
#define GDT_TYPE_DATA 0x00000200
#define GDT_TYPE_CODE 0x00000A00
#define GDT_TYPE_TSS 0x00000900
#define TSS_TYPE_B 0x00000200

//IDT
#define	IDT_FLAG_P 0x80
#define IDT_FLAG_DPL0 0x00
#define IDT_FLAG_DPL1 0x20
#define IDT_FLAG_DPL2 0x40
#define IDT_FLAG_DPL3 0x60
#define IDT_TYPE_TASK 0x05
#define IDT_TYPE_INTERRUPT 0x0E
#define IDT_TYPE_TRAP 0x0F

#define GDT_COUNT 3
#define IDT_GATED_COUNT 100
#define IST_COUNT 1

//address
#define GDTR_ADDRESS 0x00510000
#define GDT_ADDRESS (GDTR_ADDRESS + sizeof(GDTR))
#define TSS_DESCRIPTOR_ADDRESS (GDT_ADDRESS+sizeof(GDT)*GDT_COUNT)
#define TSS_ADDRESS (TSS_DESCRIPTOR_ADDRESS+sizeof(TSSDescriptor))
#define IDTR_ADDRESS (TSS_ADDRESS + sizeof(TSS))
#define IDT_ADDRESS (IDTR_ADDRESS + sizeof(IDTR))

#define IST_START_ADDRESS 0x00B00000
#define IST_SIZE 0x00100000

//descriptor offset
#define CODE_DESCRIPTOR_OFFSET 0x10
#define DATA_DESCRIPTOR_OFFSET 0x08
#define TSS_DESCRIPTOR_OFFSET 0x18

#pragma pack(push, 1)
typedef struct gdt {
    DWORD lower4Byte;
    DWORD upper4Byte;
} GDT;

typedef struct gdtr {
	WORD limit;
	QWORD baseAddress;
	WORD reserved1;
	DWORD reserved2;
} GDTR, IDTR;

typedef struct tssDescriptor {
	DWORD first4Byte;
	DWORD second4Byte;
	DWORD third4Byte;
	DWORD fourth4Byte;
}TSSDescriptor;

typedef struct tss {
	DWORD reserved1;
	QWORD rsp[3];
	QWORD reserved2;
	QWORD ist[7];
	QWORD reserved3;
	WORD reserved4;
	WORD ioMapAddress;
} TSS;

typedef struct idt {
	WORD offset0To15;
	WORD segment;
	BYTE ist;
	BYTE flag;
	WORD offset16To31;
	DWORD offset32To63;
	DWORD reserved;
} IDT;
#pragma pack(pop)

void initilizeDiscriptor(void);
void initializeGDTR(void);
void initializeGDT(void);
void initializeTSSDiscriptor(void);
void initializeTSS(void);
void initializeIDTR(void);
void initilizeIDT(void);

#endif
