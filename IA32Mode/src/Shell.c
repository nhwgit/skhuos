#include "Shell.h"
#include "String.h"
#include "portControl.h"
#include "Process.h"
#include "Disk.h"
#include "editor.h"

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

void process0(void) {
	char data[81] = "        ZZZZZZZZZZZZZZ@&$*%)@_(%&#(!)*%(@)#!ZZZZZZZZZZZZRETWZZZ8";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 0, i);
		schedule();
	}
	exitProcess();
}

void process1(void) {
	char data[81] = "        ZyZZZZZZZZZZZZ ________*&$$     yDB9ZZZZZZZZZZZZZZRETFZjZ";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 1, i);
		schedule();
	}
	exitProcess();
}

void process2(void) {
	char data[81] = "        Z                   Welcome To skhuOS,                 ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 2, i);
		schedule();
	}
	exitProcess();
}

void process3(void) {
	char data[81] = "        Z                 ZZB   EZ        ZZMHR                ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 3, i);
		schedule();
	}
	exitProcess();
}

void process4(void) {
	char data[81] = "        Z               ZZZZZ    j8y      ZDZZZZDF             ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 4, i);
		schedule();
	}
	exitProcess();
}

void process5(void) {
	char data[81] = "        Z              ZZZZEEZZZ    58     ZZZZZZZB            ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 5, i);
		schedule();
	}
	exitProcess();
}

void process6(void) {
	char data[81] = "        Z             ZZZZZZZ   DFVFDBDF   WZ      Z           Z j";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 6, i);
		schedule();
	}
	exitProcess();
}

void process7(void) {
	char data[81] = "        Z            Z  Z       8Z        BZZZZZZZZZ           ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 7, i);
		schedule();
	}
	exitProcess();
}

void process8(void) {
	char data[81] = "        Z            ZjZ wZ E   yj         5ZZZZ98Z77          ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 8, i);
		schedule();
	}
	exitProcess();
}

void process9(void) {
	char data[81] = "        Z            Z  D    ZZZZ,D           ZZEZ77           ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 9, i);
		schedule();
	}
	exitProcess();
}

void process10(void) {
	char data[81] = "        Z            Zw    BGRZZZZZ           zZZZ77           ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 10, i);
		schedule();
	}
	exitProcess();
}

void process11(void) {
	char data[81] = "        Z             Z,   DZZZXVE    ZZ       ZZ77            ZW";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 11, i);
		schedule();
	}
	exitProcess();
}

void process12(void) {
	char data[81] = "        Z              ZZ  BZB9245       ,y    DZ7             Z j";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 12, i);
		schedule();
	}
	exitProcess();
}

void process13(void) {
	char data[81] = "        Z               wZZW  j  _RGB_       ,ZZy              ZW |23";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 13, i);
		schedule();
	}
	exitProcess();
}

void process14(void) {
	char data[81] = "        Z                 RTV WEZ8EZZZYZEWGUF                  ZW   .'[]";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 14, i);
		schedule();
	}
	exitProcess();
}

void process15(void) {
	char data[81] = "        Z                                                     Zw       */OF";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 15, i);
		schedule();
	}
	exitProcess();
}

void process16(void) {
	char data[81] = "        ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZjZZZ///OOOO          \\";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 16, i);
		schedule();
	}
	exitProcess();
}

void process17(void) {
	char data[81] = "        ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ==OOOO +*          |";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 17, i);
		schedule();
	}
	exitProcess();
}

void process18(void) {
	char data[81] = "        W                                                      we    /  __|__\\";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 18, i);
		schedule();
	}
	exitProcess();
}

void process19(void) {
	char data[81] = "      yw                                                        wy   |    |  |";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 19, i);
		schedule();
	}
	exitProcess();
}

void process20(void) {
	char data[81] = "    yW@@@@@@@@@@@@@@@   D8zzzzzzzzzz95           wwwwwwwwwwwwwwww    \\ _____ /";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 20, i);
		schedule();
	}
	exitProcess();
}

void process21(void) {
	char data[81] = "  yjwwwwwwwwwwwwwwww    DyWWWWW yD               WWWWWWwwwwwwwwwwwwwwww";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 21, i);
		schedule();
	}
	exitProcess();
}

void process22(void) {
	char data[81] = " ZZZZZZZZZZZZ @$((@)$)@)#@_ %*($,,,,UIUUI  wj   BZZZZZZ    SKHUCOMPUTER";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 22, i);
		schedule();
	}
	exitProcess();
}

void process23(void) {
	char data[81] = "GRGREGBYFGL;;SDIFDK SKHU ZZANGZZANG @($*#$)@0#';[.'$2849DKFKDLS;;FDLFLDDFGJGGD";
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], 23, i);
		schedule();
	}
	exitProcess();
}

void processTest(void) {
	clear();
	createProcess((QWORD)process13);
	createProcess((QWORD)process5);
	createProcess((QWORD)process22);
	createProcess((QWORD)process23);
	createProcess((QWORD)process8);
	createProcess((QWORD)process18);
	createProcess((QWORD)process14);
	createProcess((QWORD)process19);
	createProcess((QWORD)process7);
	createProcess((QWORD)process0);
	createProcess((QWORD)process17);
	createProcess((QWORD)process15);
	createProcess((QWORD)process20);
	createProcess((QWORD)process16);
	createProcess((QWORD)process4);
	createProcess((QWORD)process11);
	createProcess((QWORD)process2);
	createProcess((QWORD)process3);
	createProcess((QWORD)process1);
	createProcess((QWORD)process6);
	createProcess((QWORD)process21);
	createProcess((QWORD)process12);
	createProcess((QWORD)process9);
	createProcess((QWORD)process10);
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
