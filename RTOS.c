#include "task.h"
#include "semaphore.h"
#include <stdlib.h>
#include "RTOS.h" // RTOS.h must be included LAST in order to avoid definition conflicts for opaque types (such as TaskHandle and SemaphoreHandle)

/********************************************************************************************
*   Returning a task handle, The StartTask() function takes the function pointer            *
*   to a task as a parameter, as well as the stack size (in 32-bit words),                  *
*   a void pointer for any arguments that the task function may need                        *
*   a pointer to a void pointer to store return values, as well as a TaskProperties object  *
*   both void * args and  void* retVal can be set to NULL if not used                       *
*********************************************************************************************/
TaskHandle CreateTask(Task task, size_t stackSize, void * args, void ** retVal, TaskProperties properties){
    TaskHandle handle = malloc(sizeof(TaskContext));
    char * stack; // cast to char for pointer arithmetic
    if (handle == NULL)
        return NULL;        // Allocation failed returns NULL
    handle->User_Properties = properties;
    stack = malloc((stackSize * 4) + 64);           // Allocates [stackSize] 32-bit (4 byte) words for stack space, plus some buffers for internal logic. 
    if (stack == NULL){
        free(handle);
        return NULL;                                // Returns NULL after freeing handle, if stack allocation fails.
    }
    handle->stackTail = stack;
    // TODO: Perform pointer arithmetic on allocated stack to store return address, and args value (for R0/arg1)
    // TODO: Add stack pointer to context buffer (at appropriate offset to be restored to the SP register)
    // TODO: Add Link Register value to be popped from task as return value if task is designed to run a return routine
    if (appendTasktoTCB(handle)){                   // Task is added to TCB. Returns NULL, if it fails to add to the TCB, function returns false.
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
}

/********************************************************************************
*   Suspends task if active, unsuspends task if suspended
*   
*********************************************************************************/
void ToggleTaskSuspend(TaskHandle handle){
    handle->User_Properties.isSuspended = !handle->User_Properties.isSuspended;
}

/********************************************************************************
*   Returns the suspend state of a task
*   
*********************************************************************************/
bool IsTaskSuspended(TaskHandle handle){
    return handle->User_Properties.isSuspended;
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
void OsInitialize(int timer){
    // This is a generic line comment to give more insight on specific lines
}

/********************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************/
void OsStart(void){
    // This is a generic line comment to give more insight on specific lines
}

/********************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************/
void OsStop(void){
    // This is a generic line comment to give more insight on specific lines
}

/********************************************************************************
*   This is a generic comment block that will tell us what the following function
*   is supposed to do.
*********************************************************************************/
void OsKill(void){
    // This is a generic line comment to give more insight on specific lines
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
    handle->taskQueue.qHead = NULL;
    handle->taskQueue.qTail = NULL;                             // taskQueue for semaphore starts NULL on both ends (nothing has been added yet)
    return handle;
}

OS_Status SemaphoreRelease(SemaphoreHandle handle){
    if (!handle)
        return osErrorParameter;                                // if bad semaphore is passed, return with error code
    handle->semaphoreState = 0;
    return osOK;
}

OS_Status SemaphoreAcquire(SemaphoreHandle handle, uint32_t timeout){
    TaskHandle task = getCurrentTask();                                // Current task is acquired in order to add to queue
    if (!handle)
        return osErrorParameter;                            // if bad semaphore is passed, return with error code
    if (!handle->semaphoreState){                           // if semaphore is free, check for other tasks already on queue, and execute if none available
        if (handle->taskQueue.qHead == NULL){
            handle->semaphoreState = 1;
            return osOK;
        }
        else{
            QueuePush(task);
            // TODO: halt task [without suspending] (ie put task to sleep until semaphire is released, and acquired by this task)
        }
    }
    // TODO: Implement Semaphore Acquire Logic
    return osOK;
}