#ifndef __PRINT_H___
#define __PRINT_H___

#include "Type.h"

#pragma pack(push, 1)
typedef struct videoCharacter {
	char character;
	char attribute;
} VideoCharacter;
#pragma pack(pop)

//video memory
#define ONE_LINE_SIZE 160
#define VIDEO_MEMORY_ADDR 0xB8000
#define ONE_CHAR_SIZE 2
#define LINE_COUNT 25

#define BACK_SPACE 0x08

void viewCharacter(BYTE ch);
void moveToNextLine(void);
void printString(const char* str);
void puts(const char* str);
void printInt(int num, int radix);
int printStateAndReturn(bool state);
void fillLastLineBlank(void);
void cursorInit(void);
void cursorLine(int line);
void setCursorMemory(int mem);
int getCurrentPoint(void);

#endif
