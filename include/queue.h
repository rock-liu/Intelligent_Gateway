#ifndef __QUEUE_H__
#define __QUEUE_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct  
{
	int x;
	int y;
}DATATYPE;
typedef struct __NODE__
{
	DATATYPE data;
	 struct __NODE__ *next;
}Node;
typedef struct __queue__ {
	Node * blank;
	Node* head;
	Node* tail;
	int tlen;
	int clen;

}LinkQueue;
typedef struct {
	int fd;
	LinkQueue* list;
}Arg;

LinkQueue* create_queue(int n);
int is_full(LinkQueue* list);
int is_emptp(LinkQueue* list);
int enter_queue(LinkQueue* list,DATATYPE ddd);
DATATYPE quie_queue(LinkQueue* list);
void destroy_queue(LinkQueue* list);

#endif
