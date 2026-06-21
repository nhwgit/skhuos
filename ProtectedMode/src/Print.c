#include "Print.h"

static int currentPoint = VIDEO_MEMORY_ADDR + ONE_LINE_SIZE*5; // 부팅 메시지 다음 줄
static const char * okSign = "OK";
static const char * failSign = "Fail";

void moveToNextLine(void) {
	int xIdx = (currentPoint-VIDEO_MEMORY_ADDR) % ONE_LINE_SIZE;
	currentPoint+=(ONE_LINE_SIZE-xIdx);
}

void viewCharacter(BYTE ch) {
	VideoCharacter * videoMemory = (VideoCharacter *)currentPoint;
	videoMemory->character = ch;
	currentPoint+=ONE_CHAR_SIZE;
}

void printString(const char* str) {
	for(int i=0; str[i]!=NULL; i++) {
		if(str[i]=='\n')
			moveToNextLine();
		else
			viewCharacter(str[i]);
	}
}

void puts(const char* str) {
	printString(str);
	moveToNextLine();
}

int printStateAndReturn(bool state) {
	if(state==TRUE)
		puts(okSign);
	else
		puts(failSign);
	return state;
}
