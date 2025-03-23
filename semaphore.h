#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

typedef struct SemaphoreContext SemaphoreContext;
typedef SemaphoreContext* SemaphoreHandle;
#include "task.h"
// Binary Semaphore Definition
typedef struct SemaphoreContext{
    uint8_t semaphoreState :1;          // 1 == acquired; 0 == released;
    QueuePointers taskQueue;
}SemaphoreContext;


#endif
