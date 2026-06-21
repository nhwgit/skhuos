#include "Shell.h"
#include "Print.h"
#include "String.h"
#include "portControl.h"
#include "Process.h"
#include "Disk.h"
#include "editor.h"
#include "Keyboard.h"
#include "Memory.h"
#include "FAT.h"

typedef struct shellCommand {
	const char * name;
	const char * usage;
	void (*func)(const char * arg);
} ShellCommand;

static void help(const char * arg);
static void clearCommand(const char * arg);
static void memUsed(const char * arg);
static void reboot(const char * arg);
static void printDiskCapacity(const char * arg);
static void multiTasking(const char * arg);
static void mountCommand(const char * arg);
static void formatting(const char * arg);
static void ls(const char * arg);
static void touch(const char * arg);
static void rm(const char * arg);
static void editor(const char * arg);
static void processTest(void);

static const ShellCommand commandTable[] = {
		{"help", "help", help},
		{"clear", "clear", clearCommand},
		{"memUsed", "memUsed", memUsed},
		{"reboot", "reboot", reboot},
		{"diskCapacity", "diskCapacity", printDiskCapacity},
		{"multiTasking", "multiTasking", multiTasking},
		{"mount", "mount", mountCommand},
		{"formatting", "formatting", formatting},
		{"ls", "ls", ls},
		{"touch", "touch fileName", touch},
		{"rm", "rm fileName", rm},
		{"editor", "editor fileName", editor}
};

static const char * accountName = "NHW";

static void printAccountName(const char * str) {
	printString(str);
	printString(">");
}

static void executeCommand(const char * cmd) {
	char name[100] = {0};
	int idx = 0;
	for(; cmd[idx]!=' ' && cmd[idx]!='\0'; idx++)
		name[idx] = cmd[idx];
	name[idx] = '\0';
	if(idx==0)
		return;
	const char * arg = (cmd[idx]==' ') ? (cmd+idx+1) : (cmd+idx);

	for(int i=0; i<sizeof(commandTable)/sizeof(ShellCommand); i++) {
		if(strcmp(name, commandTable[i].name)==0) {
			commandTable[i].func(arg);
			return;
		}
	}
	printString(cmd);
	printString(" is not found!");
}

void startShell(void) {
	int bufferIndex = 0;
	char data = 0;
	char commandBuffer[100] = {0};
	printAccountName(accountName);
	while(1) {
		if(getQueue(&data)) {
			if(data==KEY_ENTER) {
				puts("");
				executeCommand(commandBuffer);
				bufferIndex = 0;
				commandBuffer[bufferIndex] = '\0';
				puts("");
				printAccountName(accountName);
				continue;
			}
			else if(data==KEY_BACKSPACE) {
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

static void help(const char * arg) {
	for(int i=0; i<sizeof(commandTable)/sizeof(ShellCommand); i++)
		puts(commandTable[i].usage);
}

static void clearCommand(const char * arg) {
	clear();
}

static void memUsed(const char * arg) { // 램 사용량
	printMemoryRate();
}

static void reboot(const char * arg) {
	setPort(0x64, 0xD1);
	setPort(0x60, 0x00);
}

static void printDiskCapacity(const char * arg) {
	printString("Disk Capacity: ");
	printInt(getTotalSector()/2/1024, 10);
	printString("MB");
}

static void multiTasking(const char * arg) {
	processTest();
}

static void mountCommand(const char * arg) {
	if(mount())
		puts("success Mount!");
}

static void formatting(const char * arg) {
	if(format())
		puts("success disk format!");
}

static void ls(const char * arg) {
	showDir();
}

static void touch(const char * arg) {
	createFile(arg);
}

static void rm(const char * arg) {
	deleteFile(arg);
}

static void editor(const char * arg) {
	saveVideoMemory();
	runEditor(arg);
	loadVideoMemory();
}

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

static void bannerProcess(QWORD line) {
	const char * data = bannerData[line];
	for(int i=0; data[i]!=NULL && i<=80; i++) {
		int cnt=0;
		while(cnt++ != MULTITASKING_TIME);
		viewCharacterXY(data[i], line, i);
		schedule();
	}
	exitProcess();
}

static void processTest(void) {
	clear();
	for(int i=0; i<sizeof(bannerData)/sizeof(char*); i++)
		createProcess((QWORD)bannerProcess, i);
	cursorLine(24);
}
