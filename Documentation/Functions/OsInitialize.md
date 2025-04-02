# OsInitialize

The `OsInitialize` function initializes the kernel and the SysTick configuration that the RTOS will use when active. This function must be called in order to create Tasks, and of course, to start the kernel.

The `OsInitialize` function takes a 32-bit unsigned integer as a parameter, and returns an `OS_Status` enum:

```C
    OS_Status OsInitialize(uint32_t ticksPerSec);
```

The 32-bit unsigned integer indicates the number of ticks per second that the task scheduler will operate at, meaning you are selecting how often the task scheduler will check if it needs to switch tasks.

It is important to note that this relies on the SysTick handler, a function that increments uwTick which some processes may use. The programmer may need to consider this under some implementations.

It is also worth noting that by setting the number of ticks per second, doesn't necessarily indicate that tasks WILL change that often. Tasks are given a minimum amount of time to run when scheduling between tasks of the same priority. Additionally, in periods where all system tasks are idle, only the idle process will be active, or if only one task is not sleeping, only that task will continue, and task switching will not occur unless the scheduler specifically notes another task needs to run.

## Return Value

`OsInitialize` can return one of three values to indicate whether the RTOS kernel was successfully initialized, or if there was a failure in the initialization process. The values that can be returned are:

- `osOK` - Kernel Successfully initialized
- `osErrorResource` - Memory for Kernel could not be successfully allocated
- `osErrorParameter` - Bad parameter passed (ticksPerSec cannot be 0)

## Memory Allocation

The kernel initialization needs to allocate 24 words (96 bytes) to the heap. If a contiguous block of 96 bytes is not available, the function will return `osErrorResource`.

An example of how this function might be used:

```C
    // set ticks to 1khz for simplicity
    if (OsInitialize(1000) != osOK){  
        // Handle initialization failure
    }
```