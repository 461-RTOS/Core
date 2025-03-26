#include <stdlib.h>
#include "task.h"
#include "queue.h"
#include "scheduler.h"


Kernel * kernel = NULL;    // Empty, uninitialized Task Control Block


TaskHandle selectNextTask(void){
    TaskHandle nextTask = kernel->nextTask;
    kernel->currentTask = nextTask;
    kernel->nextTask = NULL;
    return nextTask;
}

TaskHandle getCurrentTask(void){
    return kernel->currentTask;
}

void setCurrentTask(TaskHandle task){
    kernel->currentTask = task;
}

void * getSaveContextPtr(TaskHandle task){		// used when saving context to buffer (Points to end of buffer so stmdb can decrement backwards to the beginning of buffer when saving)
    return (void*) &task->User_Properties;
}

void * getLoadContextPtr(TaskHandle task){		// used when loading context from buffer (points to beginning of buffer so ldmia can increment toward the end of buffer when loading)
    return (void*) &task->contextBuffer;
}

bool appendTasktoKernel(TaskHandle task){			// adds completed task initialization to Kernel. returns true on successful addition
    if (kernel == NULL){
    	return false;
    }
	TaskHandle * newTaskList = realloc(kernel->tasks, sizeof(TaskHandle) * (kernel->taskCount + 1));
    if (newTaskList == NULL){
        return false;
    }
    kernel->tasks = newTaskList;
    kernel->tasks[kernel->taskCount++] = task;
    return true;
}


void * getSaveMainContextPtr(void){		// Used when starting kernel
	return &kernel->buffer;
}

void * getLoadMainContextPtr(void){		// Used when stopping kernel
	return &kernel->mainContext;
}


void * getIdleStackPointer(void){
	return (void*) kernel->idleTask->contextBuffer.sp;
}
