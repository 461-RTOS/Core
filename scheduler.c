#include <stdlib.h>
#define RTOS__INTERNAL_     // Allows definitions of special interrupt functionns located in interrupt_config.h (in this file via its inclusion in RTOS.h)
#include "task.h"
#include "RTOS.h"
#include "main.h"

static QueuePointers taskQueue;

void SysTick_Handler(void){
    //      create task object with this function as data?
    HAL_IncTick();  // SysTick Handler should include this line for HAL functions that rely on SysTick
    QueueObject *tempTask;
    tempTask = taskQueue.qHead; //creating dummy task and setting it equal to the head
    while (tempTask->next != NULL){
        tempTask->data->lastRunTime = tempTask->data->lastRunTime +1;//if the queue isn't empty, updates the last run time counter
        if (tempTask->data->lastRunTime >= 100){
            setPendSV(); // sets pendSV to "pending", will be called as soon as no higher priority interrupts are active.
        }
        tempTask = tempTask->next;//increments through queue//if the queue isn't empty, updates the last run time counter
    }
}


