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
	mrs 	r0,		psp
	isb                         		// stops pipeline
	push 	{lr}                   		// pushes link register
	bl 		getCurrentTask           	// branch links into RTOS function to get current task parameters
	bl 		getSaveContextPtr        	// branch links into RTOS function to get save location and store to r0
	cpsid	i							// Disable interrupts to protect stack pointer
	stmdb 	r0!, 	{r4-r12, r14} 		// saves all usable registers to memory location
	str 	sp, 	[r0], 	#4			// stack pointer needs to be saved seperately because stmdb is stupid.
	cpsie	i							// Re-enable interrupts, stack pointer secure
	bl 		selectNextTask           	// branch link to RTOS function that selects next task to run
	bl 		getLoadContextPtr        	// branch lin to RTOS function to get address to new task and store to r0
	cpsid	i							// Disable interrupts to protect stack pointer
	ldmia 	r0!, 	{r4-r12, r14}  		// places register context from memory location onto usable registers
	ldr		sp,		[r0],	#4			// stack pointer needs to be loaded seperately because ldmia is stupid.	// This instruction also warns about interrupts causing problems. Has been addressed
	cpsie	i							// Re-enable interrupts, stack pointer secure
	pop {lr}                    		// pops link register
	bx lr								// branch exchange to link register (return)
