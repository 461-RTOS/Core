# OsStart and OsStop
## OsStart

The `OsStart` function hands control to the kernel, allowing the task scheduler to flag the context switcher to run when needed.

Additionally, `OsStart` creates an idle task, which is used as the entry-point to the kernel.

*See [Task Scheduler](./../Task%20Scheduler/Scheduler.md) for more information on the Idle Process*

`OsStart` takes no parameters, and returns an OS_Status signal.


```C
    OS_Status OsStart(void);
```


### Memory Allocated

When the 104 bytes are allocated for the Idle Task's Task Control Block, we also allocate a 32 word stack. This is done with the same library function `CreateTask`: 

```C
    TaskProperties properties = {PRIORITY_IDLE, 0x00, false};
    TaskHandle idleTask = CreateTask(idleProcInitializer, 32, NULL, NULL, properties);
```

As noted in the `CreateTask` section, an additional 4 bytes of stack space are allocated, for a 33-word allocation. The total memory allocation of 240 bytes. Note that the 4 byte allocation to add the task to the Kernel's task list can still lead to more fragmentation until addressed in future revisions.

### Return Value

The `OsStart` function will return one of three values to indicate the success status of running the RTOS:

- `osOK` - Kernel successfully started, and terminated at `OsStop`
- `osErrorAllocationFailure` - Failed to Allocate memory for idle task; Unable to run RTOS
- `osErrorUninitialized` - Kernel is not currently Initialized, starting RTOS without a valid kernel is not possible.

An Example of how this function might be implemented:

```C
    
    switch (OsStart()){  
        case osOK:
            // Handle Successful return
            break;
        case osErrorAllocationFailure:
            // Handle Fail to start due to allocation failure
            break;
        case osErrorUninitialized:
            // Handle Fail to start due to Kernel not initialized
            break;
    }
```

## OsStop

When a task calls OsStop, the Kernel Returns control to the main process, disabling the task scheduler.

OsStop does not take any parameters or return any values.

```C
    void OsStop(void);
```

This should resume operation from the point where `OsStart` was called.

Note that this function is not intended to be called by an interrupt, and must be called from a task directly.

Upon proper use, `OsStart` should return `osOK`