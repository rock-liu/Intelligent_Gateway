#include "head.h"


QueLink*CreateQueue()
{
    QueLink* temp =  malloc(sizeof(QueLink));
    if(NULL == temp)
    {
        perror("CreateQueue malloc");
        exit(1);
    }
    temp->head =NULL;
    temp->tail = NULL;
    temp->len = 0;
    return temp;
}
int EmptyQueue(QueLink* q)
{
    return q->len == 0;
}
int EnterQueue(QueLink* q,DATATYPE *data)
{
    Node* newnode = malloc(sizeof(Node));
    if(NULL == newnode)
    {
        perror("EnterQueue malloc");
        return -1;
    }
    memcpy(&newnode->data,data,sizeof(DATATYPE));
    newnode->next = NULL;
    if(0 == q->len)
    {
        q->head = newnode;
        q->tail = newnode;

    }
    else
    {

        q->tail->next = newnode;
        q->tail = newnode;
    }
    q->len++;
    return 0;
}

int QuitQueue(QueLink* q,DATATYPE *data)
{
    if(NULL != data)
    {
        memcpy(data,&q->head->data,sizeof(DATATYPE));
    }
    Node* temp = q->head;
    if(q->head == q->tail)
    {
        q->tail =NULL;
    }
    q->head = q->head->next;
    free(temp);
    q->len--;

    return 0;
}

int GetHead(QueLink*q, DATATYPE *data1)
{
    memcpy(data1, &q->head->data, sizeof(DATATYPE));
    return 0;
}

int ClearQueue(QueLink* q)
{
    while (q->head != NULL)
    {
        QuitQueue(q, NULL);
    }
    q->len = 0;
    return 0;
}

int DestroyQueue(QueLink** q)
{
    while(!EmptyQueue(*q))
    {
        QuitQueue(*q, NULL);
    }

    free(*q);
    *q = NULL;
    return 0;
}
