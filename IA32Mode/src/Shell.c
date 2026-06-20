#include "Shell.h"
#include "String.h"
#include "portControl.h"
#include "Process.h"
#include "Disk.h"
#include "editor.h"
#include "Keyboard.h"
#include "Memory.h"
#include "FAT.h"

static const char * accountName = "NHW";

static char * command[] = {
		"help",
		"clear",
		//"memUsed",
		"reboot",
		"diskCapacity",
		"multiTasking",
		"mount",
		"formatting",
		"ls",
		"touch fileName",
		"rm fileName",
		"editor fileName"
};

static QWORD stack[300] = {0}; // 임시
static PCB process[2] = {0};
Monitor monitor = {0};

void startShell(void) {
	int bufferIndex = 0;
	char data = 0;
	char commandBuffer[100] = {0};
	printAccountName(accountName);
	while(1) {
		if(getQueue(&data)) {
			if(data==ENTER) {
				puts("");
				executeCommand(commandBuffer);
				bufferIndex = 0;
				commandBuffer[bufferIndex] = '\0';
				puts("");
				printAccountName(accountName);
				continue;
			}
			else if(data==BACK_SPACE) {
				if(bufferIndex==0) continue;
				else commandBuffer[--bufferIndex] = '\0';
			}
			else if(data!=0) {
				commandBuffer[bufferIndex++] = data;
				commandBuffer[bufferIndex] = '\0';
			}
			viewCharacter(data);
		}
	}
}

void printAccountName(const char * str) {
	printString(str);
	printString(">");
}

void executeCommand(const char * cmd) { // 향후 간결한 형태로 개선 예정
	if(memcmp(cmd, "clear", strlen(cmd))==0)
		clear();
	else if(memcmp(cmd, "help", strlen(cmd))==0)
		help();
	else if(memcmp(cmd, "memUsed", strlen(cmd))==0)
		memUsed();
	else if(memcmp(cmd, "reboot", strlen(cmd))==0)
		reboot();
	else if(memcmp(cmd, "diskCapacity", strlen(cmd))==0)
		printDiskCapacity();
	else if(memcmp(cmd, "multiTasking", strlen(cmd))==0)
		processTest();
	else if(memcmp(cmd, "mount", strlen(cmd))==0)
		mountExecute();
	else if(memcmp(cmd, "formatting", strlen(cmd))==0)
		formatting();
	else if(memcmp(cmd, "ls", strlen(cmd))==0)
		ls();
	else if(memcmp(cmd, "touch", 5)==0)
		touch(cmd+6);
	else if(memcmp(cmd, "rm", 2)==0)
		rm(cmd+3);
	else if(memcmp(cmd, "editor", 6)==0)
		editor(cmd+7);

	else {
		printString(cmd);
		printString(" is not found!");
	}
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

void help(void) {
	for(int i=0; i<sizeof(command)/sizeof(char*); i++)
		puts(command[i]);
}

void memUsed(void) { // 램 사용량
	printMemoryRate();
}


void reboot(void) {
	setPort(0x64, 0xD1);
	setPort(0x60, 0x00);
}

/*void startProcess(void) { // 임시
	setUpProcess(&(process[1]), (QWORD)testCode, (QWORD *)&stack, sizeof(stack));
	switchContext(&(process[0].context), &(process[1].context));
}*/

static const char * bannerData[] = {
		"        ZZZZZZZZZZZZZZ@&$*%)@_(%&#(!)*%(@)#!ZZZZZZZZZZZZRETWZZZ8",
		"        ZyZZZZZZZZZZZZ ________*&$$     yDB9ZZZZZZZZZZZZZZRETFZjZ",
		"        Z                   Welcome To skhuOS,                 ZW",
		"        Z                 ZZB   EZ        ZZMHR                ZW",
		"        Z               ZZZZZ    j8y      ZDZZZZDF             ZW",
		"        Z              ZZZZEEZZZ    58     ZZZZZZZB            ZW",
		"        Z             ZZZZZZZ   DFVFDBDF   WZ      Z           Z j",
		"        Z            Z  Z       8Z        BZZZZZZZZZ           ZW",
		"        Z            ZjZ wZ E   yj         5ZZZZ98Z77          ZW",
		"        Z            Z  D    ZZZZ,D           ZZEZ77           ZW",
		"        Z            Zw    BGRZZZZZ           zZZZ77           ZW",
		"        Z             Z,   DZZZXVE    ZZ       ZZ77            ZW",
		"        Z              ZZ  BZB9245       ,y    DZ7             Z j",
		"        Z               wZZW  j  _RGB_       ,ZZy              ZW |23",
		"        Z                 RTV WEZ8EZZZYZEWGUF                  ZW   .'[]",
		"        Z                                                     Zw       */OF",
		"        ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZjZZZ///OOOO          \\",
		"        ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ==OOOO +*          |",
		"        W                                                      we    /  __|__\\",
		"      yw                                                        wy   |    |  |",
		"    yW@@@@@@@@@@@@@@@   D8zzzzzzzzzz95           wwwwwwwwwwwwwwww    \\ _____ /",
		"  yjwwwwwwwwwwwwwwww    DyWWWWW yD               WWWWWWwwwwwwwwwwwwwwww",
		" ZZZZZZZZZZZZ @$((@)$)@)#@_ %*($,,,,UIUUI  wj   BZZZZZZ    SKHUCOMPUTER",
		"GRGREGBYFGL;;SDIFDK SKHU ZZANGZZANG @($*#$)@0#';[.'$2849DKFKDLS;;FDLFLDDFGJGGD"
};

void bannerProcess(QWORD line) {
	const char * data = bannerData[line];
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], line, i);
		schedule();
	}
	exitProcess();
}

void processTest(void) {
	clear();
	for(int i=0; i<sizeof(bannerData)/sizeof(char*); i++)
		createProcess((QWORD)bannerProcess, i);
	cursorLine(24);
}

void ReadSector(void) {
	char buffer[512] = {0};
	int a = readSector(TRUE, TRUE, 1, 1, buffer);
	for(int i=0; i<512; i++)
		printInt(buffer[i], 10);
}

void WriteSector(void) {
	char buffer[512] = {0};
	for(int i=0; i<512; i++)
			buffer[i] = 'F';
	writeSector(TRUE, TRUE, 1, 1, buffer);
}

void printDiskCapacity(void) {
	printString("Disk Capacity: ");
	printInt(getTotalSector()/2/1024, 10);
	printString("MB");
}

void mountExecute(void) {
	if(mount())
		puts("success Mount!");
}

void formatting(void) {
	if(format())
		puts("success disk format!");
}

void ls(void) {
	showDir();
}

void touch(const char * name) {
	createFile(name);
}

void rm(const char * name) {
	deleteFile(name);
}

void saveVideoMemory(void) {
	VideoCharacter * videoMemory = VIDEO_MEMORY_ADDR;
	monitor.cp = getCurrentPoint();
	for(int i=0; i<4000; i++) {
		monitor.vc[i].attribute=videoMemory[i].attribute;
		monitor.vc[i].character=videoMemory[i].character;
	}
}

void loadVideoMemory(void) {
	VideoCharacter * videoMemory = VIDEO_MEMORY_ADDR;
	for(int i=0; i<4000; i++) {
		videoMemory[i].attribute = monitor.vc[i].attribute;
		videoMemory[i].character = monitor.vc[i].character;
	}
	setCursorMemory(monitor.cp);
}

void editor(const char * name) {
	saveVideoMemory();
	runEditor(name);
	loadVideoMemory();
}
