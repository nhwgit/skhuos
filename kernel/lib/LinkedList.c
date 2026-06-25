#include "lib/LinkedList.h"
#include "Type.h"
#include "Print.h"

void initList(LinkedList * list) {
	list->count = 0;
	list->head = NULL;
	list->tail = NULL;
}

void insertList(LinkedList * list, void * item) {
	Node * node = (Node *)item;
	node->next = NULL;
	if(list->head==NULL)
		list->head = item;
	else
		((Node *)(list->tail))->next = item;
	list->tail = item;
	(list->count)++;
}

void printList(LinkedList * list) {
	Node * cur = (Node*)list->head;
	int count = 1;
	while(cur!=NULL) {
		printInt(count++, 10);
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
	while(cur!=NULL) {
		if(cur->id==id) {
			if(cur == list->head && cur ==list->tail) {
				list->head = NULL;
				list->tail = NULL;
			}
			else if(cur == list->head)
				list->head = cur->next;
			else if(cur == list->tail) {
				list->tail = pre;
				pre->next = NULL;
			}
			else
				pre->next = cur->next;
			(list->count)--;
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
