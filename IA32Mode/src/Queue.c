#include "Queue.h"
#include "Memory.h"
#include "Print.h"

void initQueue(Queue * queue, void* queueBuffer, int queueLen, int size) {
	queue->front = 0;
	queue->rear = 0;
	queue->len = queueLen;
	queue->dataSize = size;
	queue->queueArr = queueBuffer;
}

bool queueIsEmpty(const Queue * queue) {
	if(queue->front == queue->rear)
		return TRUE;
	else
		return FALSE;
}

bool queueIsFull(const Queue * queue) {
	if(queue->front == ((queue->rear)+1)%queue->len)
			return TRUE;
		else
			return FALSE;
}

bool enQueue(Queue * queue, const void * data) {
	if(queueIsFull(queue))
		return FALSE;
	else {
		int size = queue->dataSize;
		int queueLen = queue->len;
		memcpy((BYTE*)queue->queueArr+(size*queue->rear), data, size);
		queue->rear = ((queue->rear)+1)%queueLen;
		return TRUE;
	}
}

bool deQueue(Queue * queue, void * data) {
	if(queueIsEmpty(queue))
		return FALSE;
	else {
		int size = queue->dataSize;
		int queueLen = queue->len;
		memcpy(data, (BYTE*)queue->queueArr+(size*queue->front), size);
		queue->front = ((queue->front)+1)%queueLen;
		return TRUE;
	}
}
