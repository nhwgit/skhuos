#include "Process.h"
#include "64bit.h"
#include "Print.h"
#include "Descriptor.h"
#include "Sync.h"
#include "ContextSwitching.h"
#include "Queue.h"
#include "Memory.h"
#include "String.h"

static WORD pidCountIdx = 0;
static ProcessScheduler scheduler = {0}; // 타이머 ISR와 공유, setIf로 보호(단일 코어)
static int allocProcessTable[PROCESS_MAXCOUNT] = {0};
static Queue exitProcessQueue = {0};
static int queueBuffer[EXIT_QUEUE_COUNT]; // pid 담는다.

void exitProcess(void) {
	bool preIf = setIf(FALSE);
	enQueue(&exitProcessQueue, &scheduler.runningProcess->link.id);
	setIf(preIf);
	schedule();
	while(1);
}

void garbageProcessCollector(void) {
	int pid;
	while(1) {
		if(!queueIsEmpty(&exitProcessQueue)) {
			bool preIf = setIf(FALSE);
			deQueue(&exitProcessQueue, &pid);
			removeList(&(scheduler.processList), pid);
			allocProcessTable[pid] = 0; // pid 슬롯 반환
			PtEntry * page = (PtEntry *)PTABLE_BASE_ADDRESS;
			page[KERNEL_SIZE+pid*2].lower4Byte &= ~PAGE_LOWER4B_FLAGS_P;
			setIf(preIf);
		}
	}
}

void setUpProcess(PCB * pcb, const QWORD entryPoint, const QWORD * stackAddress, const QWORD stackSize) {
	pcb->stackAddress = stackAddress;
	pcb->stackSize = stackSize;
	InitializeMemory(pcb->context.reg, sizeof(pcb->context.reg));
	pcb->context.reg[REG_CS] = CODE_DESCRIPTOR_OFFSET;
	pcb->context.reg[REG_SS] = DATA_DESCRIPTOR_OFFSET;
	pcb->context.reg[REG_DS] = DATA_DESCRIPTOR_OFFSET;
	pcb->context.reg[REG_ES] = DATA_DESCRIPTOR_OFFSET;
	pcb->context.reg[REG_FS] = DATA_DESCRIPTOR_OFFSET;
	pcb->context.reg[REG_GS] = DATA_DESCRIPTOR_OFFSET;
	pcb->context.reg[REG_RIP] = entryPoint;
	pcb->context.reg[REG_RSP] = (QWORD)stackAddress + stackSize;
	pcb->context.reg[REG_RBP] = (QWORD)stackAddress + stackSize;
	pcb->context.reg[REG_RFLAGS] |= RFLAGS_IF;
}

void initScheduler(void) {
	pidCountIdx = 0;
	PCB * pcb = (PCB *)(PCB_POOL_ADDRESS+sizeof(PCB)*pidCountIdx);
	pcb->link.id = pidCountIdx;
	allocProcessTable[pidCountIdx++] = 1;
	initList(&(scheduler.processList));
	scheduler.runningProcess = pcb;
	initQueue(&exitProcessQueue, queueBuffer, EXIT_QUEUE_COUNT, sizeof(int)); // link.id 크기와 일치
	createProcess((QWORD)garbageProcessCollector, 0);
}

PCB * createProcess(QWORD entryPoint, QWORD arg) { // 페이징 설정 추가 필요]
	bool preIf = setIf(FALSE);
	int tryCount = 0;
	while(allocProcessTable[(pidCountIdx)%PROCESS_MAXCOUNT]==1) {
		tryCount++;
		if(tryCount>PROCESS_MAXCOUNT) {
			puts("Create Process Error!");
			while(1);
		}
		pidCountIdx = (pidCountIdx+1)%PROCESS_MAXCOUNT;
	}
	QWORD * stackAddress = (QWORD *)(PROCESS_STACK_BASE + PROCESS_STACK_INTERVAL*(pidCountIdx));
	PCB * curProcess = (PCB *)(PCB_POOL_ADDRESS+sizeof(PCB)*pidCountIdx);
	curProcess->link.id = pidCountIdx;
	setUpProcess(curProcess, entryPoint, stackAddress, PROCESS_STACK_SIZE);
	curProcess->context.reg[REG_RDI] = arg; // 진입 함수의 첫번째 인자
	insertList(&(scheduler.processList), curProcess);
	PtEntry * page = (PtEntry *)PTABLE_BASE_ADDRESS;
	page[KERNEL_SIZE+pidCountIdx*2].lower4Byte |= PAGE_LOWER4B_FLAGS_P;
	allocProcessTable[pidCountIdx++] = 1;
	setIf(preIf);
	return curProcess;
}

void schedule(void) {
    bool preIf = setIf(FALSE); 

    if(scheduler.processList.count == 0) {
        setIf(preIf);
        return;
    }

    PCB *nextProcess = (PCB *)HeadRemove(&(scheduler.processList));

    if(nextProcess != NULL) {
        PCB *curProcess = scheduler.runningProcess;
        insertList(&(scheduler.processList), curProcess);
        scheduler.runningProcess = nextProcess;
        switchContext(&(curProcess->context), &(nextProcess->context));
    }
    setIf(preIf); 
}

void timeoutSchedule(void) {
	PCB * curProcess, *nextProcess;
	if(scheduler.processList.count==0)
		return;
	bool preIf = setIf(FALSE);
	nextProcess = (PCB *)HeadRemove(&(scheduler.processList));
	setIf(preIf);
	char * ist = (char *)(IST_START_ADDRESS+IST_SIZE-sizeof(ProcessContext));
	curProcess = scheduler.runningProcess;
	memcpy(&(curProcess->context), ist, sizeof(ProcessContext));
	preIf = setIf(FALSE);
	insertList(&(scheduler.processList), curProcess);
	setIf(preIf);
	scheduler.runningProcess = nextProcess;
	memcpy(ist, &(nextProcess->context), sizeof(ProcessContext));
}

int getRunningPid(void) {
	return (scheduler.runningProcess)->link.id;
}
