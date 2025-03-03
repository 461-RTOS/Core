#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include "RTOS.h"


typedef struct TaskContext{                                 // A pointer to this struct can be used as a task handle, for user opacity, this is typedefed sas a void pointer to typedef void* TaskHandle;
    uint8_t contextBuffer[40];                              // This identifier is used to restore context registers by the LDMIA instruction
    union{
        uint8_t contextPtr[sizeof(TaskProperties)];         // This value should never be assigned directly to, this is just a pointer to be used by STMDB instruction in order to write to the context buffer when saving context
        TaskProperties User_Properties;
    };
}TaskContext;

typedef TaskContext* TaskHandle;

// The Task control block will be used to locate currently active tasks, as well as choose which task to run when context switching
typedef struct TaskControlBlock{
    uint32_t taskCount;
    TaskHandle currentTask;
    TaskHandle * tasks;     // This is an array of pointers to tasks. We can avoid linked-list structuring, while mantaining Handle IDs if we mantain each handle's pointer, by managing an array to the pointers, rather than changing where the data itself is.
}TaskControlBlock;

TaskHandle selectNextTask(void);

TaskHandle getCurrentTask(void);

void * getSaveContextPtr(TaskHandle task);

void * getLoadContextPtr(TaskHandle task);

#endif