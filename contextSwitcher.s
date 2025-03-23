.syntax unified
	.thumb

.global PendSV_Handler
.type PendSV_Handler %function

.extern getCurrentTask
.extern getSaveContextPtr
.extern selectNextTask
.extern getLoadContextPt

.text
.align 2
PendSV_Handler:

	sub		sp,		#0x4				// decrement for alignment during call
	push 	{lr}                   		// pushes link register, needed for proper return
	bl  	getCurrentTask           	// branch links into RTOS function to get current task handle
	bl  	getSaveContextPtr        	// branch links into RTOS function, passing task handle to get pointer to save buffer (stored in r0)
	pop		{lr}						// restore link register
	add		sp,		#0x4				// restore previous alignment

	mrs 	r3,		psp					// store process stack pointer in r3 to be saved
	stmdb 	r0!, 	{r3-r11} 			// saves all usable registers to context buffer of TCB for task

	cpsid	i							// Disable interrupts to protect stack pointer integrity
	mrs		r1,		MSP					// Stack pointer saved during atomic block to avoid mangling during a possible interrupt call (undefined behavior)
	cpsie	i							// Re-enable interrupts, stack-pointer-critical section done


	ldmia	r1!,	{r2-r9}				// Pull context saved on stack (stack exception frame), move to buffer
	stmdb	r0!,	{r2-r9}


	push	{r1}						// save modified stack pointer for stack exception frame, will be needed to load new context
	push	{lr}
	bl  	selectNextTask           	// branch link to RTOS function that selects handle for next task
	bl  	getLoadContextPtr        	// branch lin to RTOS function to get address to buffer where new context is stored
	pop 	{lr}
	pop		{r1}						// restore modified stack pointer for stack exception framer

	ldmia	r0!,	{r2-r9}				// pulls context from buffer to be placed on stack exception frame
	stmdb	r1!,	{r2-r9}				// stores on stack exception frame, r1 should now match main stack pointer again

	ldmia 	r0!, 	{r3-r11}  			// places new register context from new task buffer location onto usable registers
	msr		psp,	r3					// new stack pointer in-place


	dsb									// ensures all memory operations have completed before continuing
	isb									// flushes pipeline before switching contexts
	bx lr								// branch exchange to link register (return) Stack exception frame will be restored to registers, and new context can execute
