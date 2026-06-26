#include "Print.h"
#include "String.h"
#include "util.h"

static int currentPoint = VIDEO_MEMORY_ADDR; // 각 커널 Main에서 cursorLine으로 시작 줄 지정
static const char * okSign = "OK";
static const char * failSign = "Fail";

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
		memcpy((void *)VIDEO_MEMORY_ADDR, (void *)(VIDEO_MEMORY_ADDR+ONE_LINE_SIZE),
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
	int count = 0;
	if(num==0) {
		viewCharacter('0');
		return;
	}
	if(num<0) {
		viewCharacter('-');
		num = -num;
	}
	while(num>0) {
		charNum[count++] = "0123456789ABCDEF"[num%radix];
		num /= radix;
	}
	reverseArray(charNum, count);
	for(int i=0; i<count; i++)
		viewCharacter(charNum[i]);
}

void printHex(QWORD num) {
	char charNum[16] = {0};
	int count = 0;
	if(num==0) {
		viewCharacter('0');
		return;
	}
	while(num>0) {
		charNum[count++] = "0123456789ABCDEF"[num%16];
		num /= 16;
	}
	reverseArray(charNum, count);
	for(int i=0; i<count; i++)
		viewCharacter(charNum[i]);
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

void clear(void) {
	VideoCharacter * videoMemory = (VideoCharacter *)VIDEO_MEMORY_ADDR;
	for(int i=0; i<ONE_LINE_SIZE*LINE_COUNT/sizeof(VideoCharacter); i++) {
		videoMemory->character = 0;
		videoMemory->attribute = 0x07;
		videoMemory++;
	}
	cursorInit();
}

void cursorLine(int line) {
	currentPoint=VIDEO_MEMORY_ADDR+ONE_LINE_SIZE*line;
}

typedef struct monitor { // 화면 전체와 커서 위치 백업
	VideoCharacter vc[ONE_LINE_SIZE*LINE_COUNT/ONE_CHAR_SIZE];
	int cp;
}Monitor;

static Monitor monitor = {0};

void saveVideoMemory(void) {
	VideoCharacter * videoMemory = (VideoCharacter *)VIDEO_MEMORY_ADDR;
	monitor.cp = currentPoint;
	for(int i=0; i<ONE_LINE_SIZE*LINE_COUNT/ONE_CHAR_SIZE; i++) {
		monitor.vc[i].attribute=videoMemory[i].attribute;
		monitor.vc[i].character=videoMemory[i].character;
	}
}

void loadVideoMemory(void) {
	VideoCharacter * videoMemory = (VideoCharacter *)VIDEO_MEMORY_ADDR;
	for(int i=0; i<ONE_LINE_SIZE*LINE_COUNT/ONE_CHAR_SIZE; i++) {
		videoMemory[i].attribute = monitor.vc[i].attribute;
		videoMemory[i].character = monitor.vc[i].character;
	}
	currentPoint = monitor.cp;
}
