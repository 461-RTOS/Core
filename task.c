#include <stdlib.h>
#include "task.h"


static TaskControlBlock TCB;    // Empty, uninitialized Task Control Block

TaskHandle selectNextTask(void){
    
    for (int i = 0; i != TCB.taskCount; i++){
        if (TCB.tasks[i]->lastRunTime < TCB.currentTask->lastRunTime)
            if(TCB.tasks[i]->User_Properties.priority > TCB.currentTask->User_Properties.priority)
                return TCB.tasks[i];
    }
    return NULL;                    // Returning NULL if no task is Selected
}

TaskHandle getCurrentTask(void){
    return TCB.currentTask;
}

void setCurrentTask(TaskHandle task){
    TCB.currentTask = task;
}

void * getSaveContextPtr(TaskHandle task){
    return task->contextPtr;
}

void * getLoadContextPtr(TaskHandle task){
    return task->contextBuffer;
}
