#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>
#include <stdbool.h>
typedef struct TaskContext TaskContext;
typedef TaskContext* TaskHandle;
#include "RTOS.h"
#include "queue.h"

typedef struct TaskContext{                                 // A pointer to this struct can be used as a task handle, for user opacity, this is typedefed sas a void pointer to typedef void* TaskHandle;
    uint8_t contextBuffer[44];                              // This identifier is used to restore context registers by the LDMIA instruction
    union{
        uint8_t contextPtr[sizeof(TaskProperties)];         // This value should never be assigned directly to, this is just a pointer to be used by STMDB instruction in order to write to the context buffer when saving context
        TaskProperties User_Properties;
    };
    uint32_t lastRunTime;
    void ** retval;                                         // Allows a return value pointer to be stored at a location pointed to by retVal.
    void * stackTail;                                       // Hold stack tail to free from when deleting a task
}TaskContext;

// The Task control block will be used to locate currently active tasks, as well as choose which task to run when context switching
typedef struct TaskControlBlock{
    uint32_t taskCount;             // Number of tasks in array
    TaskHandle currentTask;         // Currently Active Task
    TaskHandle * tasks;             // This is an array of pointers to tasks. We can avoid linked-list structuring, while mantaining Handle IDs if we mantain each handle's pointer, by managing an array to the pointers, rather than changing where the data itself is.
    QueuePointers taskQueue;
}TaskControlBlock;

TaskHandle selectNextTask(void);

TaskHandle getCurrentTask(void);

TaskHandle getCurrentTask(void);

void * getSaveContextPtr(TaskHandle task);

void * getLoadContextPtr(TaskHandle task);

bool appendTasktoTCB(TaskHandle task);




#endif
