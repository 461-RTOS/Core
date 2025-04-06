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
	TaskHandle handle = calloc(1, sizeof(TaskControlBlock));
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
    handle->stackHead = (void*) ((uint32_t) stack + (stackSize * 4) + 4);
    handle->retval = retVal;						// sets location to store return value
    // these values need to be initialized so when a context switch occurs, these values start the task properly for the first time.
    handle->contextBuffer.r0 = (uint32_t) args;			// Store args as parameter 1 in r0
    handle->contextBuffer.PC = ((uint32_t) task) & (~(0x00000001));			// Store beginning of function address as PC value to start executing at (need to clear thumb-bit)
    handle->contextBuffer.LR = (uint32_t) returnRoutine | 0x1;// Return Routine stored in Link Register so if function terminates, it enters a routine designed to clean up the task the bitwise OR 0x1 adds the thumb bit to the return
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
OS_Status ToggleTaskSuspend(TaskHandle handle){
	if (handle == NULL)
		return osErrorParameter;
    handle->User_Properties.isSuspended = !handle->User_Properties.isSuspended;
    handle->suspended = !handle->suspended;
    return osOK;
}

/********************************************************************************
*   Returns the suspend state of a task
*   
*********************************************************************************/
bool IsTaskSuspended(TaskHandle handle){
    return handle->suspended;
}

/********************************************************************************************
*   Removes a task from system, switches to another task if currently running -
*    Not Yet Implemented
*********************************************************************************************/
void DeleteTask(TaskHandle handle){

}

/********************************************************************************
*   Will recieve timer, just using int as placeholder
*********************************************************************************/
OS_Status OsInitialize(uint32_t ticksPerSec){
	if (ticksPerSec == 0){
		return osErrorParameter;	// cannot divide by 0, do not set tick
	}
    kernel = calloc(1, sizeof(Kernel));		// Kernel values should be initialized to 0
    if (kernel == NULL){
    	return osErrorResource;
    }
    SysTick_Init(ticksPerSec);
    return osOK;
}

void SwitchFromMain(void); // prototype needed for OsStartFunction


/********************************************************************************************
*  	Hands control to the RTOS Kernel
*********************************************************************************************/
OS_Status OsStart(void){
	if (kernel == NULL){				// Ensures Kernel is Running
		return osErrorUninitialized;
	}
	TaskProperties properties = {PRIORITY_IDLE, 0x00, false};
	TaskHandle idleTask = CreateTask(idleProcInitializer, 32, NULL, NULL, properties);	// Creates Idle Task with small stack
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

/********************************************************************************************
*   Halts Kernel Operation
*********************************************************************************************/
void OsStop(void){

	isKernelActive = false;
	SwitchToMain();
}

/********************************************************************************************
*	Uninitializes Kernel, cleaning up remaining tasks as well as TCB Not yet implemented
*********************************************************************************************/
void OsKill(void){

}

void OsDelay(uint32_t ticks){
	AtomicInternalStart();
	uint32_t currentTick = HAL_GetTick();
	TaskHandle task = getCurrentTask();
	task->delayTime = currentTick + ticks;
	task->lastRunTime = currentTick;
	task->status = TASK_WAITING;
	TaskScheduler();
	AtomicInternalStop();
	setPendSV();
	while (task->status != TASK_READY);
	return;
}

/********************************************************************************************************
SEMAPHORE FUNCTIONS
*********************************************************************************************************/



SemaphoreHandle CreateBinarySemaphore(SemaphoreAcquiredState acquiredState){      // Returns handle to semaphore. acquiredState: true == acquired, false == released
	SemaphoreHandle handle = malloc(sizeof(SemaphoreContext));
    if (!handle){
        return NULL;                                            // Returns NULL if fails to create semaphore
    }
    handle->semaphoreAcquired = acquiredState;
    handle->taskCount = 0;
    handle->tasks = NULL;
    return handle;
}


OS_Status SemaphoreRelease(SemaphoreHandle handle){
    if (!handle)
        return osErrorParameter;                                // if bad semaphore is passed, return with error code
    AtomicInternalStart();
    if (handle->semaphoreAcquired == false){								// if no tasks are available, release semaphore
		AtomicInternalStop();
		return osErrorResource;									// Resource Unavailable Error returned if semaphore already released
    }
    if (handle->taskCount == 0){								// if no tasks are available, release mutex
   		handle->semaphoreAcquired = false;
   		AtomicInternalStop();
   		return osOK;
       }
    TaskHandle bestCandidate = NULL;
    for (int i = 0; i < handle->taskCount; i++){				// if tasks are available, give one permission to acquire it (leave acquired internally)
    	TaskHandle candidate = handle->tasks[i];
    	if (candidate->suspended || candidate->status == TASK_READY)
    		continue;											// ignore tasks that have already been released but not run, and suspended tasks
    	if (bestCandidate == NULL){
    		bestCandidate = candidate;
    		continue;
    	}
    	if (candidate->priority < bestCandidate->priority)
    		bestCandidate = candidate;
    	else if (candidate->priority == bestCandidate->priority && candidate->lastRunTime <= bestCandidate->lastRunTime){
    		bestCandidate = candidate;
    	}
    }
    if (bestCandidate == NULL)
    	handle->semaphoreAcquired = false;						// if no tasks are applicable (all are suspended) free semaphore until applicable tasks are available
    else{
    	bestCandidate->status = TASK_READY;
    }
	AtomicInternalStop();
    return osOK;
}



OS_Status SemaphoreAcquire(SemaphoreHandle handle, uint32_t timeout){
    AtomicInternalStart();									// begin uninterrupted section (interrupts are okay, just no context switching)
    TaskHandle task = getCurrentTask();                                // Current task is acquired in order to add to queue
    uint32_t currentTick = HAL_GetTick();
    if (!handle){
    	AtomicInternalStop();
        return osErrorParameter;                            // if bad semaphore is passed, return with error code
    }
    if (!handle->semaphoreAcquired){                        // if semaphore is free, check for other tasks already on queue
		handle->semaphoreAcquired = true;
		AtomicInternalStop();
		return osOK;
    }
    else{
    	TaskHandle * tasksTemp = realloc(handle->tasks, sizeof(TaskHandle) * (handle->taskCount  + 1));
		if (tasksTemp == NULL){
			AtomicInternalStop();
			return osErrorAllocationFailure;
		}
		handle->tasks = tasksTemp;
		handle->tasks[handle->taskCount++] = task;
		task->delayTime = currentTick + timeout;
		task->lastRunTime = currentTick;
		task->status = TASK_BLOCKED;
		task->timeoutOccurred = false;
		task->waitingSemaphore = handle;			// give task semaphore handle Identifier while waiting
		TaskScheduler();
		AtomicInternalStop();
		setPendSV();
		while (task->status != TASK_READY);			// halt indefinitely until task switch occurs and/or task becomes ready
		AtomicInternalStart();
		int i = 0;
		for (i = 0; i < handle->taskCount; i++){
			if (handle->tasks[i] == task)
				break;								// finds current task to remove from tasks waiting on semaphore
		}
		i++;
		for (; i < handle->taskCount; i++){			// continues looping
			handle->tasks[i-1] = handle->tasks[i];	// shifts other tasks over
		}
		task->waitingSemaphore = NULL;				// remove semaphore identifier once acquired or timeout ends
		handle->tasks = realloc(handle->tasks, sizeof(TaskHandle) * (--handle->taskCount));	// frees extra memory from removed task

		if (task->timeoutOccurred){					// return with timeout error if wait times out
			AtomicInternalStop();
			return osErrorTimeout;
		}
		handle->semaphoreAcquired = true;
		AtomicInternalStop();

    }
    return osOK;
}



/********************************************************************************************************
MUTEX FUNCTIONS
*********************************************************************************************************/


MutexHandle CreateMutex(void){      // Returns handle to mutex
	MutexHandle handle = malloc(sizeof(MutexContext));
    if (!handle){
        return NULL;                                            // Returns NULL if fails to create mutex
    }

    handle->Acquired = false;
    handle->taskCount = 0;
    handle->tasks = NULL;
    handle->currentTask = NULL;
    return handle;
}


OS_Status MutexRelease(MutexHandle handle){
    if (!handle)
        return osErrorParameter;                                // if bad mutex is passed, return with error code
    AtomicInternalStart();
    if (handle->Acquired == false){								// if not acquired, return error
		AtomicInternalStop();
		return osErrorResource;									// Resource Unavailable Error returned if mutex already released
    }
    TaskHandle task = getCurrentTask();
    if (handle->currentTask != task){
    	AtomicInternalStop();
    	return osOwnershipFailure;
    }
    handle->currentTask = NULL;
    if (handle->taskCount == 0){								// if no tasks are available, release mutex
		handle->Acquired = false;
		AtomicInternalStop();
		return osOK;
    }
    TaskHandle bestCandidate = NULL;
    for (int i = 0; i < handle->taskCount; i++){				// if tasks are available, give one permission to acquire it (leave acquired internally)
    	TaskHandle candidate = handle->tasks[i];
    	if (candidate->suspended || candidate->status == TASK_READY)
    		continue;											// ignore tasks that have already been released but not run, and suspended tasks
    	if (bestCandidate == NULL){
    		bestCandidate = candidate;
    		continue;
    	}
    	if (candidate->priority < bestCandidate->priority)
    		bestCandidate = candidate;
    	else if (candidate->priority == bestCandidate->priority && candidate->lastRunTime <= bestCandidate->lastRunTime){
    		bestCandidate = candidate;
    	}
    }
    if (bestCandidate == NULL)
    	handle->Acquired = false;						// if no tasks are applicable (all are suspended) free mutex until applicable tasks are available
    else{
    	bestCandidate->status = TASK_READY;
    }
	AtomicInternalStop();
    return osOK;
}



OS_Status MutexAcquire(MutexHandle handle, uint32_t timeout){
    AtomicInternalStart();									// begin uninterrupted section (interrupts are okay, just no context switching)
    TaskHandle task = getCurrentTask();                                // Current task is acquired in order to add to queue
    uint32_t currentTick = HAL_GetTick();
    if (!handle){
    	AtomicInternalStop();
        return osErrorParameter;                            // if bad Mutex is passed, return with error code
    }
    if (!handle->Acquired){                        // if Mutex is free, check for other tasks already on queue
		handle->Acquired = true;
		handle->currentTask = task;
		AtomicInternalStop();
		return osOK;
    }
    else{
    	TaskHandle * tasksTemp = realloc(handle->tasks, sizeof(TaskHandle) * (handle->taskCount  + 1));
		if (tasksTemp == NULL){
			AtomicInternalStop();
			return osErrorAllocationFailure;
		}
		handle->tasks = tasksTemp;
		handle->tasks[handle->taskCount++] = task;
		task->delayTime = currentTick + timeout;
		task->lastRunTime = currentTick;
		task->status = TASK_BLOCKED;
		task->timeoutOccurred = false;
		task->waitingMutex = handle;			// give task mutex handle Identifier while waiting
		TaskScheduler();
		AtomicInternalStop();
		setPendSV();
		while (task->status != TASK_READY);			// halt indefinitely until task switch occurs and/or task becomes ready
		AtomicInternalStart();
		int i = 0;
		for (i = 0; i < handle->taskCount; i++){
			if (handle->tasks[i] == task)
				break;								// finds current task to remove from tasks waiting on semaphore
		}
		i++;
		for (; i < handle->taskCount; i++){			// continues looping
			handle->tasks[i-1] = handle->tasks[i];	// shifts other tasks over
		}
		task->waitingMutex = NULL;				// remove mutex identifier once acquired or timeout ends
		handle->tasks = realloc(handle->tasks, sizeof(TaskHandle) * (--handle->taskCount));	// frees extra memory from removed task
		if (task->timeoutOccurred){					// return with timeout error if wait times out
			AtomicInternalStop();
			return osErrorTimeout;
		}
		handle->Acquired = true;
		handle->currentTask = task;
		AtomicInternalStop();
    }
    return osOK;
}




/********************************************************************************************************
IDLE PROCESS Functions
*********************************************************************************************************/


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


