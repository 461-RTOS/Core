#include "task.h"


void switchHandler(void){
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
   __asm_volatile  (
   "    mrs r0,psp      \n"
   "    isb             \n"
   "                    \n"
   "    push {lr}       \n"
   "    bl getCurrentTask   \n"
   "    bl getSaveContextPtr    \n"
   "                    \n"
   "    stmdb r0!, {r4-r11, sp-lr}    \n"
   "    bl selectNextTask   \n"
   "    bl getLoadContextPtr   \n"
   "    ldmfd r0!, {r4-r11, sp-lr}  \n"
   "    pop {lr}    \n"
   );
}