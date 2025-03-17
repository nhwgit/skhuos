#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#pragma pack(push, 1)
typedef struct node {
	//struct node * next;
	void * next;
	int id;
}Node;

typedef struct linkedList {
	//Node * head;
	//Node * cur;
	//Node * tail;
	void * head;
	void * tail;
	int count;
}LinkedList;
#pragma pack(pop)

void initList(LinkedList * list);
void insertList(LinkedList * list, void * item);
void * removeList(LinkedList * list, int id);
void * HeadRemove(LinkedList * list);
void printList(LinkedList * list);
#endif
