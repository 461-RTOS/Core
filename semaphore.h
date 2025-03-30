#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

typedef struct SemaphoreContext SemaphoreContext;
typedef SemaphoreContext* SemaphoreHandle;
#include "task.h"
// Binary Semaphore Definition
typedef struct SemaphoreContext{
    TaskHandle * tasks;
    bool semaphoreAcquired;          // true = acquired; false = released;
    uint8_t taskCount;
}SemaphoreContext;


#endif
