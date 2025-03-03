void main(void){
    /* 
    if recieve: interrupt
        call function: findMemoryAddress
        insert into register 0: addressReturned
        move all main registers: address in register 0
        place: 0 in register 0
        push: 0 into all available registers
        block:pipeline
        flush:pipeline
    */
   __asm_volitile  (
   "    mrs r0,psp      \n"
   "    isb             \n"
   "                    \n"
   "    ldr r3,CurrentContext   \n"
   "    ldr r2,[r3]     \n"
   "                    \n"
   )
}