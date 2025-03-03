#ifndef TASK_H_
#define TASK_H_

#include "RTOS.h"
#include <stdint.h>

typedef struct TaskContext{                                 // A pointer to this struct can be used as a task handle, for user opacity, this is typedefed aas a void pointer to typedef void* TaskHandle;
    uint8_t contextBuffer[40];                              // This identifier is used to restore context registers by the LDMIA instruction
    union{
        uint8_t contextPtr[sizeof(TaskProperties)];         // This value should never be assigned directly to, this is just a pointer to be used by STMDB instruction in order to write to the context buffer when saving context
        TaskProperties User_Properties;
    };
}TaskContext;

#endif