#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>
#include <stdbool.h>
typedef struct TaskControlBlock TaskControlBlock;		// These definitions must be BEFORE RTOS.h is included to avoid conflicts with definitions
typedef TaskControlBlock* TaskHandle;
#include "RTOS.h"
#include "queue.h"

typedef enum TaskStatus{
	TASK_READY = 0,
	TASK_BLOCKED,
	TASK_WAITING
}TaskStatus;

typedef struct contextBuffer{ //buffer containing register values, labeled accordingly.
	uint32_t  	r0,   r1,   r2,   r3,   r12,  LR,   PC,    xPSR,   sp,
				r4,   r5,   r6,   r7,   r8,   r9,   r10,   r11;
}contextBuffer;


// Task Control Blocks contain task context
typedef struct TaskControlBlock{             	// A pointer to this struct can be used as a task handle, for user opacity, this is typedefed sas a void pointer to typedef void* TaskHandle;
	contextBuffer contextBuffer;
    TaskProperties User_Properties;
    uint32_t lastRunTime;						// tick time at last finish (or start if currently running)
    void ** retval;                          	// Allows a return value pointer to be stored at a location pointed to by retVal.
    void * stackTail;                           // Hold stack tail to free from when deleting a task
    void * stackHead;
    SemaphoreHandle waitingSemaphore;
    MutexHandle waitingMutex;
    uint32_t delayTime;							// Delay time for OsDelays / Block Timeouts
    TaskStatus status;
    bool timeoutOccurred;
    bool suspended;
    uint8_t priority;
}TaskControlBlock;

// The Kernel structure will be used to locate currently active tasks, as well as choose which task to run when context switching
typedef struct Kernel{
    uint32_t taskCount;             // Number of tasks in array
    TaskHandle currentTask;         // Currently Active Task
    TaskHandle nextTask;
    TaskHandle * tasks;             // This is an array of pointers to tasks. We can avoid linked-list structuring, while mantaining Handle IDs if we mantain each handle's pointer, by managing an array to the pointers, rather than changing where the data itself is.
    TaskHandle idleTask;
    QueuePointers taskQueue;
    contextBuffer mainContext;		// main program context can be saved here until kernel is stopped
}Kernel;

extern Kernel * kernel;

TaskHandle selectNextTask(void);

TaskHandle getCurrentTask(void);

void * getSaveContextPtr(TaskHandle task); // used when saving context to buffer (Points to end of buffer so stmdb can decrement backwards to the beginning of buffer when saving)

void * getLoadContextPtr(TaskHandle task); // used when loading context from buffer (points to beginning of buffer so ldmia can increment toward the end of buffer when loading)

bool appendTasktoKernel(TaskHandle task);     // adds completed task initialization to TCB. returns true on successful addition




#endif
