# Getting Started

OURTOS intends to be easy to use, but does still require a little setup

## What you will need:

- An STM32 Nucleo-L476RG Microcontroller
- The STM32CubeIDE
- The Contents of the Repository's `OURTOS` Directory

## Setting Up Your IDE

After STM32CubeIDE is started up and your workspace is set up, create a new project by going to:

File->New->STM32 Project 

![](./../media/gettingStarted1.png)

Navigate to the Board Selector Tab and select the Nucleo-L476RG.

![](./../media/gettingStarted2.png)

Ensure Targeted Language is set to `C`, and Targeted Binary Type is set to `Executable`, with Targeted Project Type as `STM32Cube`.

![](./../media/gettingStarted3.png)

Copy or link the files in /OURTOS/Src/ to the Project's /Core/Src/

Additionally, copy or link the files in /OURTOS/Inc/ to the Project's /Core/Inc/

![](./../media/gettingStarted4.png)

If the header files are linked to instead of copied, be sure to add their location to the project's include path.

### SysTick_Handler and PendSV_Handler

OURTOS makes use of Custom Definitions for the `SysTick_Handler` and `PendSV_Handler` routines defined in `stm32l4xx_it.c`, so we need to make sure STM32CubeMX doesn't generate them when we regenerate our code.

In the project's *.ioc file, Navigate to the "Pinout & Configuration" tab. Under Categories->System Core->NVIC switch to the `Code Generation` tab, and uncheck the `Generate IRQ handler` checkbox for `Pendable request for system service` and `Time base: System tick timer`.

![](./../media/gettingStarted5.png)


## Writing Your Code

### Including the Library

To use OURTOS, you must include the main header file in your code:

```C
#include "RTOS.h"
```

### Initializing the Kernel


In the code where you would like to set up the RTOS, you will first need to initialize the Kernel. This can be done with the `OsInitialize` function. The `OsInitialize Function will take 1 parameter for tick-rate, and return initialization status.

An implementation might look like:

```C
if (OsInitialize(1000) != osOK){
    printf("Failure to Initialize!\n");
    for(;;);
}
```

Here, we set the tick-rate to 1kHz, and check for proper initialization. If the `OsInitialize` function doesn't return `osOK`, then we notify the system of the failure. Of course, more elaborate handlers may fit your needs better.

*See [OsInitialize](./../Functions/OsInitialize.md) for more details of how to use the function.*


