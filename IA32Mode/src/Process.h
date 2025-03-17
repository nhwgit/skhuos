#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "Type.h"
#include "LinkedList.h"


#define REG_GS 0
#define REG_FS 1
#define REG_ES 2
#define REG_DS 3
#define REG_RAX 4
#define REG_RBX 5
#define REG_RCX 6
#define REG_RDX 7
#define REG_RSI 8
#define REG_RDI 9
#define REG_RBP 10
#define REG_R8 11
#define REG_R9 12
#define REG_R10 13
#define REG_R11 14
#define REG_R12 15
#define REG_R13 16
#define REG_R14 17
#define REG_R15 18
#define REG_RIP 19
#define REG_CS 20
#define REG_RFLAGS 21
#define REG_RSP 22
#define REG_SS 23

#define RFLAGS_CF 0x00000001
#define RFLAGS_PF 0x00000004
#define RFLAGS_AF 0x00000010
#define RFLAGS_ZF 0x00000040
#define RFLAGS_SF 0x00000080
#define RFLAGS_TF 0x00000100
#define RFLAGS_IF 0x00000200
#define RFLAGS_DF 0x00000400
#define RFLAGS_OF 0x00000800
#define RFLAGS_IOPL_0 0x00000000
#define RFLAGS_IOPL_1 0x00001000
#define RFLAGS_IOPL_2 0x00002000
#define RFLAGS_IOPL_3 0x00003000
#define RFLAGS_NT 0x00004000
#define RFLAGS_RF 0x00010000
#define RFLAGS_VM 0x00020000
#define RFLAGS_AC 0x00040000
#define RFLAGS_VIF 0x00080000
#define RFLAGS_VIP 0x00100000
#define RFLAGS_ID 0x00200000

#define PCB_POOL_ADDRESS 0x1000000
#define PROCESS_STACK_BASE 0x2000000
#define SLOT_TIME 5 // ms
#define PROCESS_MAXCOUNT	1024

#define EXIT_QUEUE_COUNT 100

#pragma pack(push, 1)
typedef struct processContext {
	QWORD reg[24];
} ProcessContext;

typedef struct pcb {
	Node link;
	//WORD pid;
	QWORD * pageTableAddress;
	QWORD stackSize;
	QWORD * stackAddress;
	ProcessContext context;
}PCB;

typedef struct processScheduler {
	PCB * runningProcess;
	LinkedList processList;
}ProcessScheduler;
#pragma pack(pop)

void setUpProcess(PCB * pcb, const QWORD entryPoint, const QWORD * stackAddress, const QWORD stackSize);
void testCode(void);
void initScheduler(void);
void setRunningProcess(PCB * pcb);
void addProcess(PCB * pcb);
PCB * createProcess(QWORD entry);
void schedule(void);
void timeoutSchedule(void);
void exitProcess(void);
void garbegeProcessCollector(void);
int getRunningPid(void);

#endif
