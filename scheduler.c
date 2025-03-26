#include <stdlib.h>
#define RTOS__INTERNAL_     // Allows definitions of special interrupt functionns located in interrupt_config.h (in this file via its inclusion in RTOS.h)
#include "task.h"
#include "RTOS.h"
#include "main.h"

static QueuePointers taskQueue;

bool isKernelActive = false; // must be manually set before SysTick Handler tries to schedule tasks.

void SysTick_Handler(void){
    //      create task object with this function as data?
    HAL_IncTick();  // SysTick Handler should include this line for HAL functions that rely on SysTick
    if (!isKernelActive){
    	return;				// If kernel is not set to active, Task Scheduler doesn't need to run
    }
    QueueObject *tempTask;
    tempTask = taskQueue.qHead; //creating dummy task and setting it equal to the head
    while (tempTask->next != NULL){
        ((TaskHandle)tempTask->data)->lastRunTime = ((TaskHandle)tempTask->data)->lastRunTime +1;//if the queue isn't empty, updates the last run time counter
        if (((TaskHandle)tempTask->data)->lastRunTime >= 100){
            setPendSV(); // sets pendSV to "pending", will be called as soon as no higher priority interrupts are active.
            TCB->nextTask = tempTask->data;
            return;
        }
        tempTask = tempTask->next;//increments through queue//if the queue isn't empty, updates the last run time counter
    }
}


