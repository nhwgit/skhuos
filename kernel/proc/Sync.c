#include "proc/Sync.h"
#include "arch/CPU.h"
#include "Type.h"
#include "proc/Process.h"

bool setIf(bool interrupt) {
	QWORD preRFLAGs = getRFLAGs();
	if(interrupt)
		enableInterrupt();
	else
		disableInterrupt();
	if(preRFLAGs & 0x200)
		return TRUE;
	return FALSE;
}

void initMutex(Mutex * mutex) {
	mutex->pid = -1;
	mutex->count = 0;
	initList(&(mutex->waitList));
}

void acquireLock(Mutex * mutex) {
	bool preIf = setIf(FALSE);
	int currentPid = getRunningPid();

	if(mutex->pid == currentPid)
		mutex->count++;
	else if(mutex->pid == -1) {
		mutex->pid = currentPid;
		mutex->count = 1;
	}
	else {
		// releaseLock이 소유권을 넘긴 뒤 깨우므로, 깨어난 시점엔 이미 소유자
		insertList(&(mutex->waitList), getRunningProcess());
		blockRunningProcess();
	}
	setIf(preIf);
}

void releaseLock(Mutex * mutex) {
	bool preIf = setIf(FALSE);

	if(mutex->pid != getRunningPid()) {
		setIf(preIf);
		return;
	}

	if(--mutex->count == 0) {
		PCB * next = (PCB *)HeadRemove(&(mutex->waitList));
		if(next == NULL)
			mutex->pid = -1;
		else {
			// 락을 풀지 않고 대기 큐 선두에 직접 이양 — 새 진입자의 새치기(기아) 방지
			mutex->pid = next->link.id;
			mutex->count = 1;
			wakeupProcess(next);
		}
	}
	setIf(preIf);
}
