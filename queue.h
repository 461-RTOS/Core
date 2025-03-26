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

QueuePointers taskQueueInit(TaskHandle *);

bool isQueueEmpty(void);

int queueSize(void);

void * queuePeek(void);

void * queuePop(void);

void queuePush(void * data);

#endif