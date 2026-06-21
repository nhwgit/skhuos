#ifndef __SHELL_H__
#define __SHELL_H__

#define MULTITASKING_TIME	500000

void startShell(void);
void executeCommand(const char * cmd);
void printAccountName(const char * str);
void clear(void);
void help(void);
void memUsed(void);
void reboot(void);
void processTest(void);
void printDiskCapacity(void);
void mountExecute(void);
void formatting(void);
void ls(void);
void touch(const char * name);
void rm(const char * name);
void editor(const char * name);

#endif
