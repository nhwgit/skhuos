#ifndef __SYNC_H__
#define __SYNC_H__
#include "Type.h"
#include "lib/LinkedList.h"

#pragma pack(push, 1)
typedef struct mutex {
	int pid; // 소유자 없으면 -1
	WORD count;
	LinkedList waitList; // 락 대기(BLOCKED) 프로세스 FIFO
} Mutex;
#pragma pack(pop)

bool setIf(bool interrupt);
void initMutex(Mutex * mutex);
void acquireLock(Mutex * mutex);
void releaseLock(Mutex * mutex);

#endif
