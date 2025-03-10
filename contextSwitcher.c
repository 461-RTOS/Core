#include "RTOS.h"
#include "interrupt.h"
#include "task.h"

bool taskSwitchInterrupt;//will be true if new task is chosen

void checkSwitch(void){
    if (taskSwitchInterrupt == true){
        switchHandler();
    }
}
void switchHandler(void){
    /* 
    Following function saves a current task to be ran later and places a new task from the stack into the registers
    */
   __asm_volatile  (
   "    mrs r0,psp                  \n"
   "    isb                         \n" //stops pipeline
   "                                \n"
   "    push {lr}                   \n" // pushes line register 
   "    bl getCurrentTask           \n" // branch links into RTOS function to get current task parameters
   "    bl getSaveContextPtr        \n" //branch links into RTOS function to get save location and store to r0
   "                                \n"
   "    stmdb r0!, {r4-r11, sp-lr}  \n" //saves all usable registers to memory location
   "    bl selectNextTask           \n" //branch link to RTOS function that selects next task to run
   "    bl getLoadContextPtr        \n" //branch lin to RTOS function to get address to new task and store to r0
   "    ldmfd r0!, {r4-r11, sp-lr}  \n" //places register context from memory location onto usable registers
   "    pop {lr}                    \n" //pops line register
   );
}