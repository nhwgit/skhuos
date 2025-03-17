#include "Print.h"
#include "Memory.h"

static int currentPoint=0xB8460;
static int alphabetDigit[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
char * okSign = "OK";
char * failSign = "Fail";

void moveToNextLine(void) {
	int xIdx = (currentPoint-VIDEO_MEMORY_ADDR) % ONE_LINE_SIZE;
	currentPoint+=(ONE_LINE_SIZE-xIdx);
}

void viewCharacter(BYTE ch) {
	VideoCharacter * videoMemory = (VideoCharacter *)currentPoint;
	if(ch==BACK_SPACE) {
		videoMemory-=1;
		videoMemory->character = 0;
		currentPoint-=ONE_CHAR_SIZE;
		return;
	}
	currentPoint+=ONE_CHAR_SIZE;
	if(currentPoint >= VIDEO_MEMORY_ADDR+ONE_LINE_SIZE*LINE_COUNT) {
		memcpy(VIDEO_MEMORY_ADDR, VIDEO_MEMORY_ADDR+ONE_LINE_SIZE,
				ONE_LINE_SIZE*(LINE_COUNT-1));
		fillLastLineBlank();
		videoMemory-=(ONE_LINE_SIZE/2);
		currentPoint-=ONE_LINE_SIZE;
	}
	videoMemory->character = ch;
}

void viewCharacterXY(BYTE ch, int line, int x) {
	VideoCharacter * videoMemory = (VideoCharacter *)(VIDEO_MEMORY_ADDR+line*ONE_LINE_SIZE+2*x);
	if(ch==BACK_SPACE) {
			videoMemory-=1;
			videoMemory->character = 0;
			currentPoint-=ONE_CHAR_SIZE;
			return;
	}
	videoMemory->attribute = 0x0A;
	videoMemory->character = ch;
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

void printStringLine(const char* str, int line) {
	for(int i=0; str[i]!=NULL && i<=80; i++)
		viewCharacterXY(str[i], line, i);
}

void printInt(int num, int radix) {
	char charNum[32] = {0};
	int convertConstant = '1'-1;
	int count = 0;
	if(num==0) {
		viewCharacter('0');
		return;
	}
	for(int i=0; i<32; i++)
		charNum[i]=-1;
	while(num>0) {
		int digit = num%radix;
		if(digit<10)
			charNum[count] = digit+convertConstant;
		else
			charNum[count] = alphabetDigit[digit%10];
		num /= radix;
		count++;
	}
	reverseArray(charNum, count);
	for(int i=0; i<32; i++) {
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

void fillLastLineBlank(void) {
	VideoCharacter * videoMemory = (VideoCharacter *)(VIDEO_MEMORY_ADDR+ONE_LINE_SIZE*(LINE_COUNT-1));
	for(int i=0; i<ONE_LINE_SIZE/sizeof(VideoCharacter); i++) {
		videoMemory->character = 0;
		videoMemory++;
	}
}

void cursorInit(void) {
	currentPoint=VIDEO_MEMORY_ADDR;
}

void cursorLine(int line) {
	currentPoint=VIDEO_MEMORY_ADDR+ONE_LINE_SIZE*line;
}

void setCursorMemory(int mem) {
	currentPoint = mem;
}
int getCurrentPoint(void) {
	return currentPoint;
}
