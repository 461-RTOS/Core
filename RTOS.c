#include "task.h"
#include <stdlib.h>

/********************************************************************************************
*   Returning a task handle, The StartTask() function takes the function pointer            *
*   to a task as a parameter, as well as the stack size (in 32-bit words),                  *
*   a void pointer for any arguments that the task function may need                        *
*   a pointer to a void pointer to store return values, as well as a TaskProperties object  *
*   both void * args and  void* retVal can be set to NULL if not used                       *
*********************************************************************************************/
TaskHandle CreateTask(Task task, size_t stackSize, void * args, void ** retVal, TaskProperties properties){

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

/********************************************************************************************
*   Begins an atomic block (disables timer, and appropriate interrupts) for critical sections
*   
*********************************************************************************************/
void AtomicStart(void){

}

/********************************************************************************************
*   Ends an atomic block (re-enables timer, and appropriate interrupts)
*   After critical sections are complete
*********************************************************************************************/
void AtomicStop(void){

}