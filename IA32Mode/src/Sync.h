#ifndef __SYNC_H__
#define __SYNC_H__
#include "Type.h"

#pragma pack(push, 1)
typedef struct mutex {
	volatile DWORD pid;
	volatile WORD avaliable; // TRUE/FALSE
	volatile WORD count;
} Mutex;
#pragma pack(pop)

bool setIf(bool interrupt);
void initMutex(Mutex * mutex);
void acquireLock(Mutex * mutex);
void releaseLock(Mutex * mutex);

#endif
