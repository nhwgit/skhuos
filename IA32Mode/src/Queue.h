#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "Type.h"

#pragma pack(push, 1)
typedef struct queue {
	int front;
	int rear;
	int len;
	int dataSize;
	void * queueArr;
}Queue;
#pragma pack(pop)

void initQueue(Queue * queue, void* queueBuffer, int queueLen, int size);
bool queueIsEmpty(const Queue * queue);
bool queueIsFull(const Queue * queue);
bool enQueue(Queue * queue, const void * data);
bool deQueue(Queue * queue, void * data);
#endif
