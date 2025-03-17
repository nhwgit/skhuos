#include "Sync.h"
#include "RegControl.h"
#include "Type.h"
#include "Process.h"

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
	mutex->avaliable = TRUE;
	mutex->count = 0;
	mutex->pid = -1;
}

void acquireLock(Mutex * mutex) {
	WORD pid = getRunningPid();
	while((mutex->avaliable)==FALSE && pid != mutex->pid);
	mutex->pid = pid;
	mutex->avaliable = FALSE;
	mutex->count++;
}

void releaseLock(Mutex * mutex) {
	if(mutex->count!=0) {
		mutex->count--;
		if(mutex->count==0) {
			mutex->pid = -1;
			mutex->avaliable = TRUE;
		}
	}
	else
		mutex->avaliable = TRUE;
}
