#ifndef INTERRUPT_CONFIG_H_
#define INTERRUPT_CONFIG_H_
#include <stdint.h>
#include <stdbool.h>

#ifdef RTOS__INTERNAL_		// This block is for internal use only. Can be accessed if RTOS__INTERNAL_ is defined before header is included

void SysTick_Init(uint32_t ticksPerSec);
void SysTick_Restore(void);

void PendSV_Init(void);
#endif

#endif
