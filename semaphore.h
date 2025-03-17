#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_
#include "task.h"


// Binary Semaphore Definition
typedef struct SemaphoreContext{
    uint8_t semaphoreState :1;          // 1 == acquired; 0 == released;
    QueuePointers taskQueue;
}SemaphoreContext;

typedef SemaphoreContext* SemaphoreHandle;

#endif