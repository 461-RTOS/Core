#include <stdlib.h>
#include "RTOS.h"
#include "interrupt.h"
#include "task.h"

static QueuePointers taskQueue;

void SysTick_Handler(void){
    //TODO: make this highest priority
    //      create task object with this function as data?
    //      find syntax for interrupt calls to call pendSV correctly
    QueueObject *tempTask;
    tempTask = taskQueue.qHead; //creating dummy task and setting it equal to the head
    while (tempTask->next != NULL){
        tempTask->data->lastRunTime = tempTask->data->lastRunTime +1;//if the queue isn't empty, updates the last run time counter
        if (tempTask->data->lastRunTime >= 100){
            PendSV_Handler(); // calls pendSV to pull off the current task and check for old tasks needed to run. probably should be replaced with interrupt call
        }
        tempTask = tempTask->next;//increments through queue//if the queue isn't empty, updates the last run time counter
    }
}

void PendSV_Handler(void){
    if (taskQueue.qHead != taskQueue.qTail){
        switchHandler();
    }
}

void switchHandler(void){
    /* 
    Following function saves a current task to be ran later and places a new task from the stack into the registers
    */
   __asm_volatile  (
   "    mrs r0,psp                  \n"
   "    isb                         \n" //stops pipeline
   "                                \n"
   "    push {lr}                   \n" // pushes line register 
   "    bl getCurrentTask           \n" // branch links into RTOS function to get current task parameters
   "    bl getSaveContextPtr        \n" //branch links into RTOS function to get save location and store to r0
   "                                \n"
   "    stmdb r0!, {r4-r11, sp-lr}  \n" //saves all usable registers to memory location
   "    bl selectNextTask           \n" //branch link to RTOS function that selects next task to run
   "    bl getLoadContextPtr        \n" //branch lin to RTOS function to get address to new task and store to r0
   "    ldmfd r0!, {r4-r11, sp-lr}  \n" //places register context from memory location onto usable registers
   "    pop {lr}                    \n" //pops line register
   );
}