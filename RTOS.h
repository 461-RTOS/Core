#ifndef RTOS_H_
#define RTOS_H_

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
#endif

/*Task Properties are defined as mutable, user-accessible properties*/
typedef struct TaskProperties{
    uint8_t priority;
    uint8_t permissions;
    bool isSuspended;
}TaskProperties;


// OS Status enum for signal definitions

enum OS_Status{
    osOK = 0,                       // Complete without issues
    osErrorTimeout,                 // Error: Timeout occurs before completion
    osErrorResource,                // Error: Resource currently unavailable
    osErrorParameter,               // Error: Bad Parameter passed (such as NULL)
    osErrorISR                      // Error: called from an ISR without timeout == 0
};

typedef enum OS_Status OS_Status;

/*A simple typedef for Task Function Pointers:
Task functions must take a void pointer as an argument,
and return a void pointer as well. can return NULL if return value is not used*/
typedef void * (*Task)(void *);


/********************************************************************************************
*   Returning a task handle, The StartTask() function takes the function pointer            *
*   to a task as a parameter, as well as the stack size (in 32-bit words),                  *
*   a void pointer for any arguments that the task function may need                        *
*   a pointer to a void pointer to store return values, as well as a TaskProperties object  *
*   both void * args and  void* retVal can be set to NULL if not used                       *
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
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************************/
void DeleteTask(TaskHandle handle);



/********************************************************************************************
*   Will recieve timer, just using int as placeholder
*********************************************************************************************/
void OsInitialize(int timer);



/********************************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************************/
void OsStart(void);



/********************************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************************/
void OsStop(void);



/********************************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************************/
void OsKill(void);


/*********************************************************************************************************
SEMAPHORE FUNCTIONS
*********************************************************************************************************/


SemaphoreHandle createBinarySemaphore(bool acquiredState);

OS_Status SemaphoreRelease(SemaphoreHandle handle);

OS_Status SemaphoreAcquire(SemaphoreHandle handle, uint32_t timeout);


#endif