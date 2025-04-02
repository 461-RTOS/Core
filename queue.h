#ifndef QUEUE_H_
#define QUEUE_H_

#include "RTOS.h"
#include "task.h"


typedef struct QueueObject QueueObject; 

struct QueueObject{ //struct for queue items. contains task data values and a point to the next object in the queue
    void * data;
    //QueueObject * prev;
    QueueObject * next;
};

typedef struct QueuePointers{ //struct that should be used when managing queue. contains pointers to queue objects at the head and tail
    QueueObject *qHead;
    QueueObject *qTail;
}QueuePointers;

QueuePointers* taskQueueInit(TaskHandle *);



bool isQueueEmpty(QueuePointers* userQueue);

unsigned int queueSize(QueuePointers * userQueue);

void * queuePeek(QueuePointers* userQueue);

void * queuePop(QueuePointers* userQueue);

void queuePush(void * data, QueuePointers* userQueue);

#endif
