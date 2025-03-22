#include "core_cm4.h"
#include "stm32l4xx_hal.h"
#include "interrupt_config.h"



static SysTick_Type previousSystickContext;

void SysTick_Init(uint32_t ticksPerSec){
	previousSystickContext = *SysTick;				// Saves context for later when uninitializing system.
	uint32_t reloadVal = (SystemCoreClock / ticksPerSec) - 1;
	SysTick->LOAD = reloadVal;						// Sets how often SysTick is reloaded
	NVIC_SetPriority(SysTick_IRQn, 0xFE);			// Sets SysTick to a low priority interrupt (but still higher than PendSV)
	SysTick->VAL = 0;								// Reset SysTick counter.

	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk 	|   // Use CPU clock (not external)
	                SysTick_CTRL_TICKINT_Msk   	|   // Enable interrupt
	                SysTick_CTRL_ENABLE_Msk;        // Start the counter
}

void SysTick_Restore(void){
	Systick->CTRL = 0; 								// Disable Control before writing old context
	*SysTick = previousSystickContext;
}


void PendSV_Init(void)
{

    __set_BASEPRI(0xFF); // Block PendSV Interrupts

    // 2. Clear any previous pending PendSV requests (extra safety)
    SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;

    // 3. Set PendSV to lowest priority
    NVIC_SetPriority(PendSV_IRQn, 0xFF);

    // 4. Leave BASEPRI raised until you're ready to start the scheduler
}
	}
	isAtomicRunning = false;						// Changes State before opening process to task switching.
	__set_BASEPRI(prevBasePri);
}
