#define RTOS__INTERNAL_
#include "semaphore.h"
#include "task.h"
#include <stdlib.h>
#include "RTOS.h" // RTOS.h must be included LAST in order to avoid definition conflicts for opaque types (such as TaskHandle and SemaphoreHandle)
#include "interrupt_config.h"
#include "scheduler.h"
#include "main.h"
/********************************************************************************************
*   Returning a task handle, The StartTask() function takes the function pointer            *
*   to a task as a parameter, as well as the stack size (in 32-bit words),                  *
*   a void pointer for any arguments that the task function may need                        *
*   a pointer to a void pointer to store return values, as well as a TaskProperties object  *
*   both void * args and  void* retVal can be set to NULL if not used						*
*   RTOS Kernel must be initialized before any tasks can be added							*
*********************************************************************************************/
void returnRoutine(void * retVal);
void * idleProcInitializer(void * args);

TaskHandle CreateTask(Task task, size_t stackSize, void * args, void ** retVal, TaskProperties properties){
    if (task == NULL){
    	return NULL;	// Don't even bother if you won't hand me a real task!
    }
    if (stackSize < 16){			// Stack should be at LEAST 16 words large (64 bytes, bare-minimum)
    	return NULL;
    }
	TaskHandle handle = malloc(sizeof(TaskControlBlock));
    char * stack; // cast to char for pointer arithmetic
    if (handle == NULL)
        return NULL;        // Allocation failed returns NULL
    handle->User_Properties = properties;
    handle->suspended = properties.isSuspended;
    handle->priority = properties.priority;
    handle->status  = TASK_READY;
    handle->lastRunTime = 0;
    stack = malloc((stackSize * 4) + 4);           // Allocates [stackSize] 32-bit (4 byte) words for stack space
    if (stack == NULL){
        free(handle);
        return NULL;                                // Returns NULL after freeing handle, if stack allocation fails.
    }
    handle->lastRunTime = 0;						// set to zero for now, may set to uwtick later
    handle->stackTail = stack;						// stack tail saved for cleanup (needs to be freed when task is removed)
    handle->retval = retVal;						// sets location to store return value
    // these values need to be initialized so when a context switch occurs, these values start the task properly for the first time.
    handle->contextBuffer.r0 = (uint32_t) args;			// Store args as parameter 1 in r0
    handle->contextBuffer.PC = ((uint32_t) task) & (~(0x00000001));			// Store beginning of function address as PC value to start executing at (need to clear thumb-bit)
    handle->contextBuffer.LR = (uint32_t) returnRoutine;// Return Routine stored in Link Register so if function terminates, it enters a routine designed to clean up the task
    handle->contextBuffer.sp = (uint32_t) stack + (stackSize * 4) + 4;		// tail of stack is set to stack pointer; stack pointer is incremented to the head
    handle->contextBuffer.sp -= 36;					// adjust for proper context switching
    handle->contextBuffer.sp &= ~((uint32_t) 0x7);		// lower 3 bits are cleared for 8 byte alignment of stack
    handle->contextBuffer.xPSR = 0x01000000;		// Initialize xPSR to avoid faults in context switching, bit 24 set to 1 for Thumb mode.
    if (appendTasktoKernel(handle)){                   // Task is added to TCB. Returns NULL, if it fails to add to the TCB, function returns false.
        return handle;                              // returns handle only after verifying task handle has been added to TCB
    }
    free(handle);
    free(stack);
    return NULL;                                    // If task is not successfully appended to TCB, stack and handle are freed to avoid memory leaks, and function returns NULL
}

void returnRoutine(void * retVal){                  // Return routine's address should be appeneded to the new stack where it will be popped as the return address from the task. This will save the return value in r0 register, which will be treated as arg1 of the return-oriented function call routine.
    TaskHandle currentTask = getCurrentTask();
    if (currentTask->retval != NULL){
        *(currentTask->retval) = retVal;            // if a return location was provided (e.g. not NULL), then the void pointer from a return value is stored.
    }
    // TODO: clean up task (remove task from TCB, free task context and stack), switch to another task
    // DO NOT RUN RETURN, THERE IS NOTHING TO RETURN TO. Function must initiate a routine to switch to cleanup and switch to another task.
}

/********************************************************************************
*   Updates task handle with new properties
*   
*********************************************************************************/
void ChangeTaskProperties(TaskHandle handle, TaskProperties properties){
    handle->User_Properties = properties;
    handle->suspended = properties.isSuspended;
}

/********************************************************************************
*   Suspends task if active, unsuspends task if suspended
*   
*********************************************************************************/
void ToggleTaskSuspend(TaskHandle handle){
    handle->User_Properties.isSuspended = !handle->User_Properties.isSuspended;
    handle->suspended = !handle->suspended;
}

/********************************************************************************
*   Returns the suspend state of a task
*   
*********************************************************************************/
bool IsTaskSuspended(TaskHandle handle){
    return handle->suspended;
}

/********************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************/
void DeleteTask(TaskHandle handle){
    // This is a generic line comment to give more insight on specific lines
}

/********************************************************************************
*   Will recieve timer, just using int as placeholder
*********************************************************************************/
OS_Status OsInitialize(uint32_t ticksPerSec){
    kernel = calloc(1, sizeof(TaskControlBlock));		// Kernel values should be initialized to 0
    if (kernel == NULL){
    	return osErrorResource;
    }
    SysTick_Init(ticksPerSec);
    return osOK;
}

void SwitchFromMain(void); // prototype needed for OsStartFunction
/********************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************/
OS_Status OsStart(void){
	if (kernel == NULL){
		return osErrorUninitialized;
	}
	TaskProperties properties = {PRIORITY_IDLE, 0x00, false};
	TaskHandle idleTask = CreateTask(idleProcInitializer, 128, NULL, NULL, properties);
	if (idleTask == NULL){
		return	osErrorAllocationFailure;
	}
	kernel->idleTask = idleTask;
	kernel->currentTask = idleTask;
	idleTask->lastRunTime =  HAL_GetTick();
	PendSV_Init();
	SwitchFromMain();		// Hands control to the RTOS Kernel
	return osOK;
}

void SwitchToMain(void);	// Prototype needed for OsStopFunction
/********************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************/
void OsStop(void){
    // This is a generic line comment to give more insight on specific lines
	isKernelActive = false;
	SwitchToMain();
}

/********************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************/
void OsKill(void){
    // This is a generic line comment to give more insight on specific lines
}

void OsDelay(uint32_t ticks){
	//disableScheduler();
	uint32_t Systicks = HAL_GetTick();
	TaskHandle task = getCurrentTask();
	task->delayTime = Systicks + ticks;
	task->status = TASK_WAITING;
	task->lastRunTime = Systicks;
	TaskScheduler();
	//enableScheduler();
	setPendSV();
	while (task->status != TASK_READY);
	return;
}

/********************************************************************************************************
SEMAPHORE FUNCTIONS
*********************************************************************************************************/



SemaphoreHandle createBinarySemaphore(bool acquiredState){      // Returns handle to semaphore. acquiredState: true == acquired, false == released
	SemaphoreHandle handle = malloc(sizeof(SemaphoreContext));
    if (!handle){
        return NULL;                                            // Returns NULL if fails to create semaphore
    }
    handle->semaphoreState = (acquiredState) ? 1 : 0;
    handle->taskCount = 0;
    handle->tasks = NULL;
    return handle;
}


OS_Status SemaphoreRelease(SemaphoreHandle handle){
    if (!handle)
        return osErrorParameter;                                // if bad semaphore is passed, return with error code
    handle->semaphoreState = 0;
    return osOK;
}

OS_Status SemaphoreAcquire(SemaphoreHandle handle, uint32_t timeout){
    //TaskHandle task = getCurrentTask();                                // Current task is acquired in order to add to queue
    if (!handle)
        return osErrorParameter;                            // if bad semaphore is passed, return with error code
    if (!handle->semaphoreState){                           // if semaphore is free, check for other tasks already on queue, and execute if none available
//        if (handle->taskQueue.qHead == NULL){
//            handle->semaphoreState = 1;
//            return osOK;
//        }
//        else{
//            // QueuePush(task);
//            // TODO: halt task [without suspending] (ie put task to sleep until semaphire is released, and acquired by this task)
//        }
    }
    // TODO: Implement Semaphore Acquire Logic
    return osOK;
}


// weak idle process, can be overwritten with a custom one
void __attribute__((weak)) idleProcess(void){
//	puts("Idle Process is Active!");
//	int i = 0;
	while(1){
//		if (!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){
//			OsStop();
//		}
//		printf("It's Working!!! (%d)\n", i++);
		// HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin));
	}
}

// initializes idle process during kernel start
void * idleProcInitializer(void * args){	// args and return values unused; just part of the function type tasks use
	isKernelActive = true;					// idle process initializer will allow SysTick to begin task switching
	idleProcess();
	return NULL;
}


