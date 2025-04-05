#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

typedef struct SemaphoreContext SemaphoreContext;
typedef SemaphoreContext* SemaphoreHandle;
typedef struct MutexContext MutexContext;
typedef MutexContext* MutexHandle;
#include "task.h"
// Binary Semaphore Definition
typedef struct SemaphoreContext{
    TaskHandle * tasks;
    bool semaphoreAcquired;          // true = acquired; false = released;
    uint8_t taskCount;
}SemaphoreContext;

// Binary Semaphore Definition
typedef struct MutexContext{
    TaskHandle * tasks;
    TaskHandle currentTask;
    bool Acquired;          // true = acquired; false = released;
    uint8_t taskCount;
}MutexContext;

#endif
