# OURTOS

The Opportunistic and Uncompromised Real-Time Operating System is a Real-Time Operating system focused on several core principles:

- Ease of Use
- Scalability
- Priority-Based Task Scheduling
- Predictable Behavior
- Well-defined memory-use

Based on these principles, OURTOS intends to be an easy-to-use General-Purpose RTOS.



## Functions

There are many Functions exposed to the programmer to abstract away from the Nuances of the RTOS. This section intends to explain the functionality of each function, while still making the memory utilization of the system clear for the user.

Many of the functions in this library may rely on heap memory, so the programmer should keep memory fragmentation in mind when ordering functions that rely on an allocation of heap memory.



### OsInitialize

The `OsInitialize` function initializes the kernel and the SysTick configuration that the RTOS will use when active. This function must be called in order to create Tasks, and of course, to start the kernel.

The `OsInitialize` function takes a 32-bit unsigned integer as a parameter, and returns an `OS_Status` enum:

```C
    OS_Status OsInitialize(uint32_t ticksPerSec);
```

The 32-bit unsigned integer indicates the number of ticks per second that the task scheduler will operate at, meaning you are selecting how often the task scheduler will check if it needs to switch tasks.

It is important to note that this relies on the SysTick handler, a function that increments uwTick which some processes may use. The programmer may need to consider this under some implementations.

It is also worth noting that by setting the number of ticks per second, doesn't necessarily indicate that tasks WILL change that often. Tasks are given a minimum amount of time to run when scheduling between tasks of the same priority. Additionally, in periods where all system tasks are idle, only the idle process will be active, or if only one task is not sleeping, only that task will continue, and task switching will not occur unless the scheduler specifically notes another task needs to run.

`OsInitialize` can return one of three values to indicate whether the RTOS kernel was successfully initialized, or if there was a failure in the initialization process. The values that can be returned are:

- `osOK` - Kernel Successfully initialized
- `osErrorResource` - Memory for Kernel could not be successfully allocated
- `osErrorParameter` - Bad parameter passed (ticksPerSec cannot be 0)

The kernel initialization needs to allocate 24 words (96 bytes) to the heap. If a contiguous block of 96 bytes is not available, the function will return `osErrorResource`.

An example of how this function might be used:

```C
    // set ticks to 1khz for simplicity
    if (OsInitialize(1000) != osOK){  
        // Handle initialization failure
    }
```

### CreateTask

The `CreateTask` function is used to create the structure for a task's Task Control Block, as well as adding that task to the list of tasks the kernel keeps track of.

The `CreateTask` function takes 5 parameters:

```C
    TaskHandle CreateTask(Task task, size_t stackSize, void * args, void ** retVal, TaskProperties properties);
```

- The function the task should use as an entry point
- the size of the stack the task should have access to (in 4-byte words)
- Arguments to be passed to the entry function
- A location to store the return value
- A struct containing the details of other Task Properties

#### Argument 1: Task
The first argument of the function is the function pointer of the Task to be started, or rather the function entry point.

This function must be designed to take a void pointer as a parameter, and return a void pointer as defined by the task typedef:

```C
    typedef void * (*Task)(void*);
```

#### Argument 2: Stack Size
The second argument is the stack size in 4-byte words. The programmer must evaluate how much stack space the task may need in order to avoid stack overflows while still using memory sparingly.

According the the AAPCS states that the stack must be 8-byte aligned on function calls. However, memory allocation is only aligned to 4-byte boundaries. To compensate for this, 4 bytes are added to the stack allocation in order to ansure the stack available to the task is at least what was requested from the programmer. This is then aigned to the 8-byte address boundary by clearing the lower three bits of the stack pointer for the task, thus decrementing the stack pointer up to 4 bytes to ensure alignment compliance with the AAPCS.

In mantaining this 8-byte alignment while preserving volatile registers during an interrupt call including context switches, the interrupt decrements the process stack pointer (PSP) by up to 36 bytes (32 if already aligned) when saving context.

The programmer is responsible for allocating at least enough stack space for the interrupt to save context with (9 words). Realistically, most stacks will need to be much larger than this size, particularly if the task is not written in assembly where the user cannot manage stack usage as precisely.

#### Argument 3: Arguments

#### Argument 4: Return Storage Location

#### Argument 5: Task Properties

#### Return Value: TaskHandle

#### Memory Allocated