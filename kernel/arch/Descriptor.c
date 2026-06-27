#include "lib/64bit.h"
#include "arch/RegControl.h"
#include "arch/Descriptor.h"
#include "arch/Handler.h"

void initializeDescriptor(void) {
	initializeGDTR();
	initializeGDT();
	initializeTSSDescriptor();
	initializeTSS();
	initializeIDTR();
	initializeIDT();
	loadGDTR(GDTR_ADDRESS);
	loadTR(TSS_DESCRIPTOR_OFFSET);
	loadIDTR(IDTR_ADDRESS);
}

void initializeIDTR(void) {
	IDTR * idtr = (IDTR *)IDTR_ADDRESS;
	WORD size = (sizeof(IDT)*IDT_GATED_COUNT)-1;
	idtr->limit = size;
	idtr->baseAddress = IDT_ADDRESS;
	idtr->reserved1 = 0;
	idtr->reserved2 = 0;
}

void initializeIDT(void) {
	IDT * idt = (IDT *)IDT_ADDRESS;
	for(int i=0; i<IDT_GATED_COUNT; i++) {
		QWORD isr = (i<INTERRUPT_VECTOR_COUNT) ? vectorStubTable[i] : vectorStubTable[RESERVED_VECTOR];
		idt[i].offset0To15 = isr & 0x0000FFFF;
		idt[i].segment = CODE_DESCRIPTOR_OFFSET;
		idt[i].ist = IST_COUNT & 0x07;
		idt[i].flag = IDT_FLAG_P | IDT_FLAG_DPL0 | IDT_TYPE_INTERRUPT;
		idt[i].offset16To31 = (isr & 0xFFFF0000) >> 16;
		idt[i].offset32To63 = isr >> 32;
		idt[i].reserved = 0;
	}
}

void initializeGDTR(void) {
	GDTR * gdtr = (GDTR *)GDTR_ADDRESS;
	int size = sizeof(GDT)*GDT_COUNT + sizeof(TSSDescriptor)-1;
	gdtr->limit = size;
	gdtr->baseAddress = GDT_ADDRESS;
	gdtr->reserved1 = 0;
	gdtr->reserved2 = 0;
}

void initializeGDT(void) {
	GDT * gdt = (GDT *)GDT_ADDRESS;
	DWORD segmentSize = 0x000FFFFF;
	gdt[0].lower4Byte = 0;
	gdt[0].upper4Byte = 0;
	gdt[1].lower4Byte = segmentSize & 0x0000FFFF;
	gdt[1].upper4Byte = GDT_FLAG_P | GDT_FLAG_DPL0 | GDT_FLAG_G | GDT_FLAG_L | GDT_FLAG_S | \
			GDT_TYPE_DATA  | (segmentSize & 0xF0000);
	gdt[2].lower4Byte = segmentSize & 0x0000FFFF;
	gdt[2].upper4Byte = GDT_FLAG_P | GDT_FLAG_DPL0 | GDT_FLAG_G | GDT_FLAG_L | GDT_FLAG_S | \
			GDT_TYPE_CODE  | (segmentSize & 0xF0000);
}

void initializeTSSDescriptor(void) {
	TSSDescriptor * tssDescriptor = (TSSDescriptor *)TSS_DESCRIPTOR_ADDRESS;
	DWORD segmentSize = sizeof(TSS)-1;
	tssDescriptor->first4Byte = ((segmentSize)&0xFFFF) | ((TSS_ADDRESS & 0x0000FFFF) << 16);
	tssDescriptor->second4Byte = ((TSS_ADDRESS &0x00FF0000) >> 16) | \
				(GDT_FLAG_P | GDT_FLAG_G | GDT_FLAG_DPL0 | GDT_TYPE_TSS) | \
				(TSS_ADDRESS & 0xFF000000) | (segmentSize & 0xF0000);
	tssDescriptor->third4Byte = (QWORD)TSS_ADDRESS >> 32;
	tssDescriptor->fourth4Byte = 0;
}

void initializeTSS(void) {
	TSS * tss = (TSS *)TSS_ADDRESS;
	InitializeMemory(TSS_ADDRESS, TSS_ADDRESS+sizeof(TSS));
	tss->ist[0] = IST_START_ADDRESS+IST_SIZE;
	tss->ioMapAddress = 0xFFFF;
}
