.syntax unified
	.thumb

.global PendSV_Handler
.type PendSV_Handler %function
.global	SwitchFromMain
.type SwitchFromMain %function
.global SwitchToMain
.type SwitchToMain %function



.extern getCurrentTask
.extern getSaveContextPtr
.extern selectNextTask
.extern getLoadContextPtr
.extern getSaveMainContextPtr
.extern getLoadMainContextPtr
.extern getIdleStackPointer
.extern idleProcInitializer

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

	mrs		r1,		psp


	ldmia	r1!,	{r2-r9}				// Pull context saved on stack (stack exception frame), move to buffer
	stmdb	r0!,	{r2-r9}


	push	{r1}						// save modified stack pointer for stack exception frame, will be needed to load new context
	push	{lr}
	bl  	selectNextTask           	// branch link to RTOS function that selects handle for next task
	bl  	getLoadContextPtr        	// branch link to RTOS function to get address to buffer where new context is stored
	pop 	{lr}
	pop		{r1}						// restore modified stack pointer for stack exception framer

	ldmia	r0!,	{r2-r10}			// pulls context from buffer to be placed on stack exception frame
	msr		psp,	r10					// new stack pointer in-place
	add		r10,	0x20
	stmdb	r10!,	{r2-r9}				// stores on stack exception frame,

	ldmia 	r0!, 	{r4-r11}  			// places new register context from new task buffer location onto usable registers


	dsb									// ensures all memory operations have completed before continuing
	isb									// flushes pipeline before switching contexts
	bx lr								// branch exchange to link register (return) Stack exception frame will be restored to registers, and new context can execute






.align 2
SwitchFromMain:							// Used to shift context to idle task

	sub 	sp, 	#0x4
	push	{LR}
	bl		getSaveMainContextPtr
	stmdb	r0!,	{r4-r11}			// save r4 - r11 to main context save in kernel
	sub		r0,		#0x4				// sub 4 to point to sp

	mrs		r1,		xPSR
	str		r1,		[r0, #-4]!			// sub 4 more before to store xPSR
	sub		r0,		#0x4				// sub 4 more to point to PC (to skip)
	ldr		r4,		[sp]				// copy link register from stack
	mov		r3,		r12
	stmdb	r0!,	{r3-r4}				// saves link register, and r12
//	Previous context should be mostly saved by this point. No need to save r0-r3, as they are volatile scratch registers

	bl getIdleStackPointer				// get idle stack pointer to store in psp

	pop		{LR}
	add		sp,		#0x4				// restore

	msr		psp,	r0					// Store Idle task's stack pointer to PSP
	mov		r0,		#0x2
	msr		CONTROL, r0					// Switch to PSP

	dsb
	isb
	B		idleProcInitializer			// Start Idle Process and task Scheduler


.align 2
SwitchToMain:

	mov		r0,		#0x0
	msr		CONTROL, r0					// Switch back to main stack pointer

	bl	getLoadMainContextPtr

	ldmia	r0!,	{r3-r4}				// Restore R12 (in r3) and Link Register (in r4)
	mov 	r12,	r3
	mov		LR,		r4

	add		r0,		#0x4				// skip address of PC, we don't care about it here

	ldr		r1,		[r0], #0x4			// Restore program status register
	// msr		xPSR,	r1				// Don't know why I was trying to save it; it is volatile between function calls anyways
	add		r0,		#0x4				// skip stack pointer (not saved, as existing stack pointer MSR shouldn't be mangled)

	ldmia	r0!,	{r4-r11}

	bx		lr							// return to OsStart after SwitchFromMain was called







