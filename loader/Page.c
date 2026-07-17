#include "Page.h"
#include "Print.h"

void InitializePaging(void) {
   InitializePML4Table();
   InitializePageDirectoryPointerTable();
   InitializePageDirectoryTable();
   InitializePageTable();
}

// 상위 레벨(PML4·PDPT·PD)은 U/S를 열어 두고 최종 접근 권한은 PTE의 U/S 비트로만 결정한다
// (유저 접근 허용은 AND 조건이라 상위에서 막으면 PTE 단위 제어가 불가능)
void InitializePML4Table(void) {
	Pml4Entry * pml4Entry = (Pml4Entry *)PML4_BASE_ADDRESS;
	pml4Entry->lower4Byte = PAGE_LOWER4B_FLAGS_P | PAGE_LOWER4B_FLAGS_RW \
							| PAGE_LOWER4B_FLAGS_US | PDPTABLE_BASE_ADDRESS;
	pml4Entry->upper4Byte = (PDPTABLE_BASE_ADDRESS >> 28) & 0xFF;
	for(int i=1; i<TABLE_COUNT; i++) {
		pml4Entry[i].lower4Byte = 0x0;
		pml4Entry[i].upper4Byte = 0x0;
	}
}

void InitializePageDirectoryPointerTable(void) { //단위 : gb
	PdpEntry * pdpEntry = (PdpEntry *)PDPTABLE_BASE_ADDRESS;
	for(int i=0; i<MAX_MEMORY_SIZE; i++) {
		int pageTableAddress = PDTABLE_BASE_ADDRESS + (i*PAGE_TABLE_SIZE);
		pdpEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_P | PAGE_LOWER4B_FLAGS_RW \
							| PAGE_LOWER4B_FLAGS_US | pageTableAddress;
		pdpEntry[i].upper4Byte = (pageTableAddress >> 28) & 0xFF;
	}
	for(int i=MAX_MEMORY_SIZE; i<TABLE_COUNT; i++) {
		pdpEntry[i].lower4Byte = 0x0;
		pdpEntry[i].upper4Byte = 0x0;
	}
}

void InitializePageDirectoryTable(void) {
	PdEntry * pdEntry = (PdEntry *)PDTABLE_BASE_ADDRESS;
	for(int i=0; i<MAX_MEMORY_SIZE*TABLE_COUNT; i++) {
		int pageTableAddress = PTABLE_BASE_ADDRESS + (i*PAGE_TABLE_SIZE);
		pdEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_P | PAGE_LOWER4B_FLAGS_RW \
				| PAGE_LOWER4B_FLAGS_US | pageTableAddress;
		pdEntry[i].upper4Byte = (pageTableAddress >> 28) & 0xFF;
	}

}

void InitializePageTable(void) { // 커널영역 이후 기본적으로 활성화 안돼있다.
	PtEntry * ptEntry = (PtEntry *)PTABLE_BASE_ADDRESS;
	for(int i=0; i<KERNEL_SIZE*512; i++) {
		int physicalAddress = i*0x1000;
		ptEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_P | PAGE_LOWER4B_FLAGS_RW \
				| physicalAddress;
		ptEntry[i].upper4Byte = ((physicalAddress >> 28) & 0xFF);
	}

	for(int i=KERNEL_SIZE*512; i<MAX_MEMORY_SIZE*TABLE_COUNT*512; i++) {
		int physicalAddress = i*0x1000;
		ptEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_RW \
				| physicalAddress;
		ptEntry[i].upper4Byte = ((physicalAddress >> 28) & 0xFF);
	}
	for(int i=MAX_CODE_SIZE*TABLE_COUNT*512; i<MAX_MEMORY_SIZE*TABLE_COUNT*512; i++) {
		int physicalAddress = i*0x1000;
		ptEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_RW \
				| physicalAddress;
		ptEntry[i].upper4Byte = ((physicalAddress >> 28) & 0xFF) | PAGE_UPPER4B_FLAGS_EXB;
	}
}

/*void InitializePageDirectoryTable(void) {
	PdEntry * pdEntry = (PdEntry *)PDTABLE_BASE_ADDRESS;
	for(int i=0; i < TABLE_COUNT*MAX_MEMORY_SIZE; i++) {
		int physicalAddress = i*0x200000;
		pdEntry[i].lower4Byte = PAGE_LOWER4B_FLAGS_P | PAGE_LOWER4B_FLAGS_RW \
				| (PDIRECTORY_LOWER4B_FLAGS_PS | physicalAddress);
		pdEntry[i].upper4Byte = (physicalAddress >> 32) & 0xFF;
	}
}*/
