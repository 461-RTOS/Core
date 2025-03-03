.global _start
_start:
	
	msr r0, psp @saving stack pointer
	isb
	
	ldr r3, pxcurrentTCBConst @variable is defined in C. contains last task control block. possibly rename this to be distinct
	ldr r2, [r3]
	@would include FPU but current board does not support it
	stmdb r0!, {r4-r11, r14} @saves core registers to psp
	str r0, [r2] @writes to the address of currentTCB
	
	stmdb sp!, {r0, r3}
	mov r0, %0
	msr basepri, r0
	dsb
	isb
	bl vTaskSwitchContext @ c function that decides which context to load
	
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r0, r3} 
	
	ldr r1, [r3] 
	ldr r0, [r1]
	
	ldmia r0!, {r4-r11, r14} @pop core registers
	
	msr psp, r0 @update psp
	isb
	
	 bx r14 