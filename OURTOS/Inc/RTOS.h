#ifndef RTOS_H_
#define RTOS_H_
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "interrupt_config.h"


#ifndef TASK_H_ // This helps avoid typedef conflicts where TaskHandle is defined elsewhere in a non-opaque manner
/*Abstracted from user as a void pointer, Task handle will contain
the unique Task ID, the stack size (in 32-bit words), saved register contexts,
 as well as other immutable, or user-inaccessible data.*/
typedef void* TaskHandle;
#endif

#ifndef SEMAPHORE_H_
typedef void * SemaphoreHandle;
typedef void * MutexHandle;
#endif

/*Task Properties are defined as mutable, user-accessible properties*/
typedef struct TaskProperties{
    uint8_t priority;
    uint8_t permissions;
    bool isSuspended;
}TaskProperties;


// OS Status enum for signal definitions

typedef enum OS_Status{
    osOK = 0,                       // Complete without issues
    osErrorTimeout,                 // Error: Timeout occurs before completion
    osErrorResource,                // Error: Resource currently unavailable
    osErrorParameter,               // Error: Bad Parameter passed (such as NULL)
    osErrorISR,                     // Error: called from an ISR without timeout == 0
	osErrorUninitialized,			// Error: RTOS Kernel is not currently initialized
	osErrorAllocationFailure,		// Error: failure to allocate resource memory
	osOwnershipFailure				// Error: task does not own current resource
}OS_Status;

typedef enum OS_Status OS_Status;

typedef enum TaskPriority{
	PRIORITY_REALTIME = 0x00,
	PRIORITY_VERY_HIGH,
	PRIORITY_HIGH,
    PRIORITY_HIGH_NORMAL,
	PRIORITY_NORMAL,
    PRIORITY_LOW_NORMAL,
	PRIORITY_LOW,
	PRIORITY_IDLE = 0xFF
}TaskPriority;

typedef bool SemaphoreAcquiredState;
#define SEMAPHORE_STATE_ACQUIRED true
#define SEMAPHORE_STATE_RELEASED false

/*A simple typedef for Task Function Pointers:
Task functions must take a void pointer as an argument,
and return a void pointer as well. can return NULL if return value is not used*/
typedef void * (*Task)(void *);


/********************************************************************************************
*   Returning a task handle, The StartTask() function takes the function pointer            *
*   to a task as a parameter, as well as the stack size (in 32-bit words),                  *
*   a void pointer for any arguments that the task function may need                        *
*   a pointer to a void pointer to store return values, as well as a TaskProperties object  *
*   both void * args and  void* retVal can be set to NULL if not used						*
*   RTOS Kernel must be initialized before any tasks can be added							*
*********************************************************************************************/
TaskHandle CreateTask(Task task, size_t stackSize, void * args, void ** retVal, TaskProperties properties);



/********************************************************************************
*   Updates task handle with new properties
*
*********************************************************************************/
void ChangeTaskProperties(TaskHandle handle, TaskProperties properties);



/********************************************************************************
*   Suspends task if active, unsuspends task if suspended
*
*********************************************************************************/
void ToggleTaskSuspend(TaskHandle handle);



/********************************************************************************
*   Returns the suspend state of a task
*
*********************************************************************************/
bool IsTaskSuspended(TaskHandle handle);



/********************************************************************************************
*   Removes a task from system, switches to another task if currently running -
*    Not Yet Implemented
*********************************************************************************************/
void DeleteTask(TaskHandle handle);



/********************************************************************************************
*   Initializes the kernel
*********************************************************************************************/
OS_Status OsInitialize(uint32_t ticksPerSec);



/********************************************************************************************
*  	Hands control to the RTOS Kernel
*********************************************************************************************/
OS_Status OsStart(void);



/********************************************************************************************
*   Halts Kernel Operation
*********************************************************************************************/
void OsStop(void);



/********************************************************************************************
*	Uninitializes Kernel, cleaning up remaining tasks as well as TCB Not yet implemented
*********************************************************************************************/
void OsKill(void);

void OsDelay(uint32_t ticks);
/*********************************************************************************************************
SEMAPHORE FUNCTIONS
*********************************************************************************************************/


SemaphoreHandle CreateBinarySemaphore(SemaphoreAcquiredState acquiredState);

OS_Status SemaphoreRelease(SemaphoreHandle handle);

OS_Status SemaphoreAcquire(SemaphoreHandle handle, uint32_t timeout);

/*********************************************************************************************************
MUTEX FUNCTIONS
*********************************************************************************************************/

MutexHandle CreateMutex(void);

OS_Status MutexRelease(MutexHandle handle);

OS_Status MutexAcquire(MutexHandle handle, uint32_t timeout);
#endif
