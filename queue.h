#ifndef QUEUE_H_
#define QUEUE_H_

#include "RTOS.h"
#include "task.h"


typedef struct QueueObject QueueObject; 

struct QueueObject{
    TaskHandle data;
    QueueObject * next;
};

typedef struct QueuePointers{
    QueueObject *qHead;
    QueueObject *qTail;
}QueuePointers;

TaskHandle QueuePop(void);

void QueuePush(TaskHandle task);

#endif
