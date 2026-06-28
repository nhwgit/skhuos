#include "proc/Process.h"
#include "lib/64bit.h"
#include "Print.h"
#include "arch/Descriptor.h"
#include "arch/HandlerImp.h"
#include "arch/CPU.h"
#include "drivers/PIC.h"
#include "proc/Sync.h"
#include "arch/ContextSwitching.h"
#include "lib/Queue.h"
#include "mm/Memory.h"
#include "String.h"

static WORD pidCountIdx = 0;
static ProcessScheduler scheduler = {0}; // 타이머 ISR와 공유, setIf로 보호(단일 코어)
static int allocProcessTable[PROCESS_MAXCOUNT] = {0};
static Queue exitProcessQueue = {0};
static int queueBuffer[EXIT_QUEUE_COUNT]; // pid 담는다.
static PCB * collectorProcess = NULL;
static volatile QWORD tickCount = 0; // 타이머 ISR가 증가

QWORD getTickCount(void) {
	return tickCount;
}

PCB * getRunningProcess(void) {
	return scheduler.runningProcess;
}

// 현재 프로세스를 준비 리스트에 되돌리지 않고 전환. IF=0 전제
static void scheduleBlocked(void) {
	PCB * cur = scheduler.runningProcess;
	PCB * next = (PCB *)HeadRemove(&(scheduler.processList));
	if(next == NULL)
		next = scheduler.idleProcess;
	next->state = PROCESS_STATE_RUNNING;
	scheduler.runningProcess = next;
	switchContext(&(cur->context), &(next->context));
}

void blockRunningProcess(void) {
	scheduler.runningProcess->state = PROCESS_STATE_BLOCKED;
	scheduleBlocked();
}

void sleepUntilTick(QWORD tick) {
	PCB * cur = scheduler.runningProcess;
	cur->wakeupTick = tick;
	insertList(&(scheduler.sleepList), cur);
	blockRunningProcess();
}

void sleepProcess(QWORD ms) {
	bool preIf = setIf(FALSE);
	sleepUntilTick(tickCount + (ms + SLOT_TIME - 1) / SLOT_TIME);
	setIf(preIf);
}

bool wakeupProcess(PCB * pcb) {
	if(pcb == NULL || pcb->state != PROCESS_STATE_BLOCKED)
		return FALSE;
	removeList(&(scheduler.sleepList), pcb->link.id); // sleep 대기가 아니면 no-op
	pcb->state = PROCESS_STATE_READY;
	insertList(&(scheduler.processList), pcb);
	return TRUE;
}

void wakeupProcessInInterrupt(PCB * pcb) {
	if(!wakeupProcess(pcb))
		return;
	if(scheduler.runningProcess == scheduler.idleProcess)
		timeoutSchedule(); // IST 컨텍스트 교체 — 핸들러 복귀(iretq) 시 깨어난 쪽으로 전환
}

void exitProcess(void) {
	setIf(FALSE); // 소멸하는 프로세스라 복원하지 않음
	PCB * cur = scheduler.runningProcess;
	enQueue(&exitProcessQueue, &cur->link.id);
	wakeupProcess(collectorProcess);
	cur->state = PROCESS_STATE_EXIT;
	scheduleBlocked();
	while(1);
}

void garbageProcessCollector(void) {
	int pid;
	while(1) {
		bool preIf = setIf(FALSE);
		while(queueIsEmpty(&exitProcessQueue))
			blockRunningProcess(); // exitProcess가 깨움
		deQueue(&exitProcessQueue, &pid);
		allocProcessTable[pid] = 0; // pid 슬롯 반환
		PtEntry * page = (PtEntry *)PTABLE_BASE_ADDRESS;
		page[KERNEL_SIZE+pid*2].lower4Byte &= ~PAGE_LOWER4B_FLAGS_P;
		setIf(preIf);
	}
}

static void idleProcessLoop(void) {
	while(1)
		halt(); // 인터럽트가 올 때까지 CPU 정지
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

// 만료 시각이 지난 sleep 프로세스를 준비 리스트로 되돌림. ISR 문맥 전제
static void wakeExpiredProcess(void) {
	Node * cur = (Node *)scheduler.sleepList.head;
	while(cur != NULL) {
		Node * next = (Node *)cur->next; // wakeup이 리스트에서 빼므로 미리 저장
		PCB * pcb = (PCB *)cur;
		if(tickCount >= pcb->wakeupTick)
			wakeupProcess(pcb);
		cur = next;
	}
}

static void timerInterruptHandler(int vectorNumber) {
	tickCount++;
	sendEOI(vectorNumber - IRQ_START_OFFSET);
	wakeExpiredProcess();
	timeoutSchedule();
}

void initScheduler(void) {
	pidCountIdx = 0;
	PCB * pcb = (PCB *)(PCB_POOL_ADDRESS+sizeof(PCB)*pidCountIdx);
	pcb->link.id = pidCountIdx;
	pcb->state = PROCESS_STATE_RUNNING;
	allocProcessTable[pidCountIdx++] = 1;
	initList(&(scheduler.processList));
	initList(&(scheduler.sleepList));
	scheduler.runningProcess = pcb;
	initQueue(&exitProcessQueue, queueBuffer, EXIT_QUEUE_COUNT, sizeof(int)); // link.id 크기와 일치
	collectorProcess = createProcess((QWORD)garbageProcessCollector, 0);
	scheduler.idleProcess = createProcess((QWORD)idleProcessLoop, 0);
	removeList(&(scheduler.processList), scheduler.idleProcess->link.id); // idle은 준비 리스트 밖에서 관리
	registerInterruptHandler(IRQ_START_OFFSET+IRQ_TIMER, timerInterruptHandler);
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
	curProcess->state = PROCESS_STATE_READY;
	insertList(&(scheduler.processList), curProcess);
	PtEntry * page = (PtEntry *)PTABLE_BASE_ADDRESS;
	page[KERNEL_SIZE+pidCountIdx*2].lower4Byte |= PAGE_LOWER4B_FLAGS_P;
	allocProcessTable[pidCountIdx++] = 1;
	setIf(preIf);
	return curProcess;
}

void schedule(void) {
    bool preIf = setIf(FALSE);

    PCB *nextProcess = (PCB *)HeadRemove(&(scheduler.processList));

    if(nextProcess != NULL) {
        PCB *curProcess = scheduler.runningProcess;
        curProcess->state = PROCESS_STATE_READY;
        insertList(&(scheduler.processList), curProcess);
        nextProcess->state = PROCESS_STATE_RUNNING;
        scheduler.runningProcess = nextProcess;
        switchContext(&(curProcess->context), &(nextProcess->context));
    }
    setIf(preIf);
}

// ISR 문맥 전제(IF=0, 컨텍스트가 IST에 저장된 상태)
void timeoutSchedule(void) {
	PCB * curProcess, *nextProcess;
	if(scheduler.processList.count==0)
		return;
	nextProcess = (PCB *)HeadRemove(&(scheduler.processList));
	char * ist = (char *)(IST_START_ADDRESS+IST_SIZE-sizeof(ProcessContext));
	curProcess = scheduler.runningProcess;
	memcpy(&(curProcess->context), ist, sizeof(ProcessContext));
	if(curProcess != scheduler.idleProcess) { // idle은 준비 리스트가 빌 때만 실행
		curProcess->state = PROCESS_STATE_READY;
		insertList(&(scheduler.processList), curProcess);
	}
	nextProcess->state = PROCESS_STATE_RUNNING;
	scheduler.runningProcess = nextProcess;
	memcpy(ist, &(nextProcess->context), sizeof(ProcessContext));
}

int getRunningPid(void) {
	return (scheduler.runningProcess)->link.id;
}
