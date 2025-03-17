#ifndef __PRINT_H___
#define __PRINT_H___

#include "Type.h"

#pragma pack(push, 1)
typedef struct videoCharacter {
	BYTE character;
	BYTE attribute;
} VideoCharacter;
#pragma pack(pop)

//video memory
#define ONE_LINE_SIZE 160
#define VIDEO_MEMORY_ADDR 0xB8000
#define ONE_CHAR_SIZE 2

void viewCharacter(BYTE ch);
void moveToNextLine(void);
void printString(const char* str);
void puts(const char* str);
void printInt(int num);
int printStateAndReturn(bool state);

#endif
