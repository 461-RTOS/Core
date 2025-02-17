#ifndef RTOS_H_
#define RTOS_H_

#include <stdint.h>
#include <stdbool.h>


/*Abstracted from user as a void pointer, Task handle will contain
the unique Task ID, the stack size (in 32-bit words), saved register contexts,
 as well as other immutable, or user-inaccessible data.*/
typedef void* TaskHandle;

/*Task Properties are defined as mutable, user-accessible properties*/
typedef struct TaskProperties{
    uint8_t priority;
    uint8_t permissions;
    bool isSuspended;
}TaskProperties;


/*A simple typedef for Task Function Pointers:
Task functions must take a void pointer as an argument,
and return a void pointer as well. can return NULL if return value is not used*/
typedef void * (*Task)(void *);

/*Returning a task handle, The StartTask() function takes the function pointer 
to a task as a parameter, as well as the stack size (in 32-bit words),
a void pointer for any arguments that the task function may need
a pointer to a void pointer to store return values, as well as a TaskProperties object
both void * args and  void* retVal can be set to NULL if not used*/
TaskHandle CreateTask(Task task, size_t stackSize, void * args, void ** retVal, TaskProperties properties);


void ChangeTaskProperties(TaskHandle handle, TaskProperties properties);

void ToggleTaskSuspend(TaskHandle handle);


#endif