#include "Process.h"
#include "64bit.h"
#include "Print.h"
#include "Descriptor.h"
#include "Sync.h"
#include "ContextSwitching.h"
#include "Queue.h"
#include "Memory.h"

static WORD pidCountIdx = 0;
static ProcessScheduler scheduler = {0};
static allocProcessTable[PROCESS_MAXCOUNT] = {0};
static Queue exitProcessQueue = {0};
static QWORD queueBuffer[100]; // pid 담는다.
static Mutex processListMutex = {0};

void testCode(void) {
	int i=0;
	puts("Process execute");
	while(1);
}

void exitProcess(void) {
	enQueue(&exitProcessQueue, &scheduler.runningProcess->link.id);
	schedule();
	while(1);
}

void garbegeProcessCollector(void) {
	int pid;
	while(1) {
		if(!queueIsEmpty(&exitProcessQueue)) {
			deQueue(&exitProcessQueue, &pid);
			removeList(&(scheduler.processList), pid);
			PtEntry * page = (PtEntry *)PT_ENTRY_ADDRESS;
			page[KERNEL_SIZE+pid*2].lower4Byte &= ~PAGE_LOWER4B_FLAGS_P;
		}
	}
}

void setUpProcess(PCB * pcb, const QWORD entryPoint, const QWORD * stackAddress, const QWORD stackSize) {
	//pcb->pid = (pidCount++)%PROCESS_MAXCOUNT;
	//pcb->pid = pid;
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
	initMutex(&processListMutex);
	PCB * pcb = (PCB *)(PCB_POOL_ADDRESS+sizeof(PCB)*pidCountIdx);
	pcb->link.id = pidCountIdx;
	allocProcessTable[pidCountIdx++] = 1;
	initList(&(scheduler.processList));
	scheduler.runningProcess = pcb;
	initQueue(&exitProcessQueue, queueBuffer, EXIT_QUEUE_COUNT, sizeof(QWORD));
	createProcess((QWORD)garbegeProcessCollector);
}

PCB * createProcess(QWORD entryPoint) { // 페이징 설정 추가 필요]
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
	QWORD * stackAddress = (QWORD *)(PROCESS_STACK_BASE + 0x3000*(pidCountIdx));
	PCB * curProcess = (PCB *)(PCB_POOL_ADDRESS+sizeof(PCB)*pidCountIdx);
	curProcess->link.id = pidCountIdx;
	setUpProcess(curProcess, entryPoint, stackAddress, 0x2000);
	//acquireLock(&processListMutex);
	insertList(&(scheduler.processList), curProcess);
	PtEntry * page = (PtEntry *)PT_ENTRY_ADDRESS;
	page[KERNEL_SIZE+pidCountIdx*2].lower4Byte |= PAGE_LOWER4B_FLAGS_P;
	allocProcessTable[pidCountIdx++] = 1;
	setIf(preIf);
	//releaseLock(&processListMutex);
	return curProcess;
}

void schedule(void) {
	PCB * curProcess, *nextProcess;
	if(scheduler.processList.count==0)
		return;
	//acquireLock(&processListMutex);
	bool preIf = setIf(FALSE);
	nextProcess = (PCB *)HeadRemove(&(scheduler.processList));
	//releaseLock(&processListMutex);
	setIf(preIf);
	if(nextProcess!=NULL) {
		curProcess = scheduler.runningProcess;
		//acquireLock(&processListMutex);
		preIf = setIf(FALSE);
		insertList(&(scheduler.processList), curProcess);
		//releaseLock(&processListMutex);
		setIf(preIf);
		scheduler.runningProcess = nextProcess;
		switchContext(&(curProcess->context), &(nextProcess->context));
	}
	//setIf(preIf);
}

void timeoutSchedule(void) {
	PCB * curProcess, *nextProcess;
	//bool preIf = setIf(FALSE);
	if(scheduler.processList.count==0)
		return;
	bool preIf = setIf(FALSE);
	//acquireLock(&processListMutex);
	nextProcess = (PCB *)HeadRemove(&(scheduler.processList));
	//releaseLock(&processListMutex);
	setIf(preIf);
	char * ist = (char *)(IST_START_ADDRESS+IST_SIZE-sizeof(ProcessContext));
	curProcess = scheduler.runningProcess;
	memcpy(&(curProcess->context), ist, sizeof(ProcessContext));
	//acquireLock(&processListMutex);
	preIf = setIf(FALSE);
	insertList(&(scheduler.processList), curProcess);
	//releaseLock(&processListMutex);
	setIf(preIf);
	scheduler.runningProcess = nextProcess;
	memcpy(ist, &(nextProcess->context), sizeof(ProcessContext));
}

int getRunningPid(void) {
	return (scheduler.runningProcess)->link.id;
}
