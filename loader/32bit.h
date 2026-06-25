#ifndef __32BIT_H___
#define __32BIT_H___

//main memory

#define PROTECTED_START_ADDRESS 0x10000
#define PAGE_TABLE_START_ADDRESS 0x100000
#define IA32_START_ADDRESS 0x600000
#define IA32_END_ADDRESS 0xA00000
int InitializeMemory(int start, int end);
void copyIA32ModeImage(void);


#endif
