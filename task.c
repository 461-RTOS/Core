#include <stdlib.h>
#include "task.h"


static TaskControlBlock TCB;    // Empty, uninitialized Task Control Block

TaskHandle selectNextTask(void){
    return NULL;                    // Returning NULL until logic to select next task is implemented
}

TaskHandle getCurrentTask(void){
    return TCB.currentTask;
}

void * getSaveContextPtr(TaskHandle task){
    return task->contextPtr;
}

void * getLoadContextPtr(TaskHandle task){
    return task->contextBuffer;
}