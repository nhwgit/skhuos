#include "LinkedList.h"
#include "Type.h"

void initList(LinkedList * list) {
	list->count = 0;
	list->head = NULL;
	list->tail = NULL;
}

void insertList(LinkedList * list, void * item) {
	//Node * cur = (Node *)item;
	//cur->next = NULL;
	(list->count)++;
	if(list->head==NULL) {
		list->head = item;
		list->tail = item;
	}
	Node * cur = (Node *)list->tail;
	cur->next=item;
	list->tail = item;
	((Node *)(list->tail))->next = NULL;
}

void printList(LinkedList * list) {
	Node * cur = (Node*)list->head;
	int count = 1;
	while(cur!=NULL) {
		printInt(count++);
		printString(": ");
		printInt(cur->id, 10);
		puts("");
		cur = cur->next;
	}
	puts("end print");
}
void * removeList(LinkedList * list, int id) {
	Node * pre = (Node*)list->head;
	Node * cur = pre;
	(list->count)--;
	while(cur!=NULL) {
		if(cur->id==id) {
			if(cur == list->head && cur ==list->tail) {
				list->head = NULL;
				list->tail = NULL;
			}
			else if(cur == list->head)
				list->head = cur->next;
			else if(cur == list->tail)
				list->tail = pre;
			else
				pre->next = cur->next;
			return cur;
		}
		pre = cur;
		cur = cur->next;
	}
	return NULL;
}

void * HeadRemove(LinkedList * list) {
	if(list->count ==0)
		return NULL;
	Node * cur = (Node *)(list->head);
	return removeList(list, cur->id);
}
