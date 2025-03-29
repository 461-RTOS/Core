#ifndef INTERRUPT_CONFIG_H_
#define INTERRUPT_CONFIG_H_
#include <stdint.h>
#include "main.h"




// Enum to be passed to AtomicStart based on needs of Critical Section
typedef enum AtomicBlockLevel{
	ATOMIC_BLOCK_TASK_SCHEDULER = (0x0F << (8U - __NVIC_PRIO_BITS)),		// Blocks Task Scheduler (PendSV)
	ATOMIC_BLOCK_SYSTICK = (0x0E << (8U - __NVIC_PRIO_BITS)),			// Blocks SysTick and Task Scheduler
	ATOMIC_BLOCK_ALL = (0x01 << (8U - __NVIC_PRIO_BITS)),					// Blocks all interrupts except priority 0
}AtomicBlockLevel;

// Enum to indicate state of Atomic Block Initialization or Deinitialization
typedef enum AtomicStatus{
	ATOMIC_OK = 0x0,						// Indicates Atomic Block was properly started or stopped
	ATOMIC_NESTED_ERROR,					// Indicates Atomic Block was started while another Atomic Block was active
	ATOMIC_FLOW_ERROR						// Indicates an attempt to Terminate an Atomic Block without an Atomic Block Currently Active
}AtomicStatus;

/*********************************************************************
* AtomicStart and AtomicStop are intended to block interrupts at
* critical sections. An Atomic Block must ALWAYS open with AtomicStart
* and must ALWAYS end with AtomicStop.
*
* Atomic Blocks CANNOT be nested within on another.
***********************************************************************/
AtomicStatus AtomicStart(AtomicBlockLevel level);
AtomicStatus AtomicStop(void);

#ifdef RTOS__INTERNAL_		// This block is for internal use only. Can be accessed if RTOS__INTERNAL_ is defined before header is included

void SysTick_Init(uint32_t ticksPerSec);
void SysTick_Restore(void);

void PendSV_Init(void);

void setPendSV(void);
#endif

#endif
