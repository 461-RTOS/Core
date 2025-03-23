#include <stdlib.h>
#include "task.h"
#include "queue.h"
#include "scheduler.h"


static TaskControlBlock TCB;    // Empty, uninitialized Task Control Block


TaskHandle selectNextTask(void){
    QueueObject *tempTask;
    tempTask = TCB.taskQueue.qHead; //creating temptask to check queue for valid tasks starting from head
   while (tempTask->next != NULL){
        if (tempTask->data->lastRunTime < TCB.currentTask->lastRunTime){
            TCB.currentTask=tempTask->data;//set up new task as the current task
            return tempTask->data;
        }
        tempTask = tempTask->next;//increment to next task in queue
   };
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

bool appendTasktoTCB(TaskHandle task){
    TaskHandle * newTaskList = realloc(TCB.tasks, sizeof(TaskHandle) * (TCB.taskCount + 1));
    if (newTaskList == NULL){
        return false;
    }
    TCB.tasks = newTaskList;
    TCB.tasks[TCB.taskCount++] = task;
    return true;
}
