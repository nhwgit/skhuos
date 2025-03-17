#ifndef __SHELL_H__
#define __SHELL_H__
#include "Print.h"

#define ENTER		0x0A
#define BACK_SPACE	0x08
#define MULTITASKING_TIME	500000

typedef struct monitor {
	VideoCharacter vc[4000];
	int cp;
}Monitor;
const char * accountName = "NHW";
void startShell(void);
void executeCommand(const char * cmd);
void printAccountName(const char * str);
void help(void);
void memUsed(void);
void reboot(void);
void startProcess(void);
void printDiskCapacity(void);
void mountExecute(void);
void formatting(void);
void ls(void);
void touch(const char * name);
void rm(const char * name);

#endif
