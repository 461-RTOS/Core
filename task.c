#include <stdlib.h>
#include "task.h"


static TaskControlBlock TCB;    // Empty, uninitialized Task Control Block

TaskHandle selectNextTask(void){
    int i = 0;
    while (i != sizeof(TCB.tasks)){
        if (TCB.tasks[i]->lastRunTime < TCB.currentTask->lastRunTime)
            return TCB.tasks[i];
    }
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
