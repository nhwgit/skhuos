#include "64bit.h"
#include "RegControl.h"
#include "Descriptor.h"
#include "Handler.h"

static QWORD isrAddress[48] = {
		(QWORD)IV00, (QWORD)IV01, (QWORD)IV02, (QWORD)IV03, // 0~3
		(QWORD)IV04, (QWORD)IV05, (QWORD)IV06, // 4~6
		(QWORD)IV07, (QWORD)IV08, // 7~8
		(QWORD)IV09, (QWORD)IV10, // 9~10
		(QWORD)IV11, (QWORD)IV12, // 11~12
		(QWORD)IV13, (QWORD)IV14, (QWORD)IV15, // 13~15
		(QWORD)IV16, (QWORD)IV17, (QWORD)IV18, // 16~18
		(QWORD)IV19, (QWORD)IV20, (QWORD)IV20, // 19~21
		(QWORD)IV20, (QWORD)IV20, (QWORD)IV20, // 22~24
		(QWORD)IV20, (QWORD)IV20, (QWORD)IV20, // 25~27
		(QWORD)IV20, (QWORD)IV20, (QWORD)IV20, // 28~30
		(QWORD)IV20, // 31
		(QWORD)IV32, (QWORD)IV33, (QWORD)IV34, (QWORD)IV35, // 32~35
		(QWORD)IV36, (QWORD)IV37, (QWORD)IV38, // 36~38
		(QWORD)IV39, (QWORD)IV40, (QWORD)IV41, // 39~41
		(QWORD)IV42, (QWORD)IV43, (QWORD)IV44, // 42~44
		(QWORD)IV45, (QWORD)IV46, (QWORD)IV47 // 45~47
};
void initilizeDiscriptor(void) {
	initializeGDTR();
	initializeGDT();
	initializeTSSDiscriptor();
	initializeTSS();
	initializeIDTR();
	initilizeIDT();
}

void initializeIDTR(void) {
	IDTR * idtr = (IDTR *)IDTR_ADDRESS;
	WORD size = (sizeof(IDT)*IDT_GATED_COUNT)-1;
	idtr->limit = size;
	idtr->baseAddress = IDT_ADDRESS;
	idtr->reserved1 = 0;
	idtr->reserved2 = 0;
}

void initilizeIDT(void) {
	IDT * idt = (IDT *)IDT_ADDRESS;
	for(int i=0; i<sizeof(isrAddress)/sizeof(QWORD); i++) {
		idt[i].offset0To15 = isrAddress[i] & 0x0000FFFF;
		idt[i].segment = CODE_DESCRIPTOR_OFFSET;
		idt[i].ist = IST_COUNT & 0x07;
		idt[i].flag = IDT_FLAG_P | IDT_FLAG_DPL0 | IDT_TYPE_INTERRUPT;
		idt[i].offset16To31 = (isrAddress[i]& 0xFFFF0000) >> 16;
		idt[i].offset32To63 = (isrAddress[i] >> 32);
		idt[i].reserved = 0;
	}
	for(int i=sizeof(isrAddress)/sizeof(QWORD); i<IDT_GATED_COUNT; i++) {
		idt[i].offset0To15 = (QWORD)IV20 & 0x0000FFFF;
		idt[i].segment = CODE_DESCRIPTOR_OFFSET;
		idt[i].ist = IST_COUNT & 0x07;
		idt[i].flag = IDT_FLAG_P | IDT_FLAG_DPL0 | IDT_TYPE_INTERRUPT;
		idt[i].offset16To31 = ((QWORD)IV20& 0xFFFF0000) >> 16;
		idt[i].offset32To63 = ((QWORD)IV20 >> 32);
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

void initializeTSSDiscriptor(void) {
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
