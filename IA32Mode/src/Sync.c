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
    WORD currentPid = getRunningPid();

    if (mutex->avaliable == FALSE && mutex->pid == currentPid) {
        mutex->count++;
        return;
    }

    while (__sync_bool_compare_and_swap(&(mutex->avaliable), TRUE, FALSE) == FALSE) {
        schedule();
    }
    mutex->pid = currentPid;
    mutex->count = 1;
}

void releaseLock(Mutex * mutex) {
    WORD currentPid = getRunningPid();

    if (mutex->pid != currentPid || mutex->avaliable == TRUE) {
        return;
    }

    mutex->count--;

    if (mutex->count == 0) {
        mutex->pid = -1;
        __sync_synchronize(); 
        mutex->avaliable = TRUE;
    }
}