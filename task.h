#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>
#include <stdbool.h>
typedef struct TaskContext TaskContext;
typedef TaskContext* TaskHandle;
#include "RTOS.h"
#include "queue.h"

typedef struct contextBuffer{
	uint32_t  	xPSR, PC,   LR,   r12,  r3,   r2,   r1,    r0,   sp,
				r4,   r5,   r6,   r7,   r8,   r9,   r10,   r11;
}contextBuffer;

typedef struct TaskContext{                                 // A pointer to this struct can be used as a task handle, for user opacity, this is typedefed sas a void pointer to typedef void* TaskHandle;
	contextBuffer contextBuffer;
    TaskProperties User_Properties;
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

void * getSaveContextPtr(TaskHandle task); // used when saving context to buffer (Points to end of buffer so stmdb can decrement backwards to the beginning of buffer when saving)

void * getLoadContextPtr(TaskHandle task); // used when loading context from buffer (points to beginning of buffer so ldmia can increment toward the end of buffer when loading)

bool appendTasktoTCB(TaskHandle task);




#endif
