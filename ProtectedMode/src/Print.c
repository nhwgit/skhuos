#include "Print.h"

static int currentPoint=0xB8320;
char * okSign = "OK";
char * failSign = "Fail";

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

void printInt(int num) {
	char charNum[10] = {0};
	int convertConstant = '1'-1;
	for(int i=0; i<10; i++)
		charNum[i]=-1;
	while(num>0) {
		charNum[num/10] = (num%10)+convertConstant;
		num /= 10;
	}
	for(int i=0; i<10; i++) {
		if(charNum[i]!=-1)
			viewCharacter(charNum[i]);
		else
			break;
	}
}

int printStateAndReturn(bool state) {
	if(state==TRUE)
		puts(okSign);
	else
		puts(failSign);
	return state;
}
