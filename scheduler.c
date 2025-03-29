#include <stdlib.h>
#define RTOS__INTERNAL_     // Allows definitions of special interrupt functionns located in interrupt_config.h (in this file via its inclusion in RTOS.h)
#include "task.h"
#include "RTOS.h"
#include "main.h"

static QueuePointers taskQueue;

bool isKernelActive = false; // must be manually set before SysTick Handler tries to schedule tasks.


bool TaskScheduler(void){	// Returns true if new task has been found; false if new task has not been found.
	TaskHandle currentTask = kernel->currentTask;
	uint32_t currentTick = HAL_GetTick();

	for (uint32_t i = 0; i < kernel->taskCount; i++){
		TaskHandle candidate = kernel->tasks[i];

		if (!candidate->suspended && candidate->status == TASK_WAITING){
			if (currentTick >= candidate->delayTime){
				currentTask = candidate;
				currentTask->status = TASK_READY;
				kernel->nextTask = currentTask;
				return true;										// Tasks stopped with delays are resumed under high priority
			}
		}


		if (candidate == kernel->currentTask)						// Don't bother checking current task
			continue;

		if (!candidate->suspended && candidate->status == TASK_READY){	// Only consider tasks that aren't suspended and are flagged "Ready"
			if (candidate->priority < currentTask->priority){
				currentTask = candidate;							// If a task isn't blocked, and is higher priority, it unquestionably becomes most likely candidate
			}
			else if (candidate->priority == currentTask->priority && candidate->lastRunTime <= currentTask->lastRunTime){
				currentTask = candidate;						// If the priority is the same as current task, it checks which task has waited longer since last run
			}
		}
	}

	if (currentTask == kernel->currentTask){
		return false;														// don't switch tasks if candidate is active task
	}
	if(kernel->currentTask->priority > currentTask->priority){
			kernel->nextTask = currentTask;							// if candidate is higher priority than active task, set to next task and queue switch
			return true;
		}
	if(currentTask->priority == kernel->currentTask->priority && kernel->currentTask->lastRunTime >= (HAL_GetTick() - 10)){
		kernel->nextTask = currentTask;		// if current active task is same priority as candidate, switch if active task has overstayed its welcome and queue switch
		return true;
	}
	return false;
}

void SysTick_Handler(void){
    //      create task object with this function as data?
    HAL_IncTick();  // SysTick Handler should include this line for HAL functions that rely on SysTick
    if (!isKernelActive){
    	return;				// If kernel is not set to active, Task Scheduler doesn't need to run
    }

    if(TaskScheduler()){
    	setPendSV();
    }
    	// A simple task scheduler design to test
//    QueueObject *tempTask;
//    tempTask = taskQueue.qHead; //creating dummy task and setting it equal to the head
//    while (tempTask->next != NULL){
//        ((TaskHandle)tempTask->data)->lastRunTime = ((TaskHandle)tempTask->data)->lastRunTime +1;//if the queue isn't empty, updates the last run time counter
//        if (((TaskHandle)tempTask->data)->lastRunTime >= 100){
//            setPendSV(); // sets pendSV to "pending", will be called as soon as no higher priority interrupts are active.
//            kernel->nextTask = tempTask->data;
//            return;
//        }
//        tempTask = tempTask->next;//increments through queue//if the queue isn't empty, updates the last run time counter
//    }
}


