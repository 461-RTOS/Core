#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#include "RTOS.h"
#include "task.h"
#include "queue.h"

void SysTick_Handler(void);

void PendSV_Handler(void);

void switchHandler(void);


#endif
