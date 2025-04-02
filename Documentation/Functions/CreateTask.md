# CreateTask

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

## Argument 1: Task

The first argument of the function is the function pointer of the Task to be started, or rather the function entry point.

This function must be designed to take a void pointer as a parameter, and return a void pointer as defined by the task typedef:

```C
    typedef void * (*Task)(void*);
```

## Argument 2: Stack Size

The second argument is the stack size in 4-byte words. The programmer must evaluate how much stack space the task may need in order to avoid stack overflows while still using memory sparingly.

According the the AAPCS states that the stack must be 8-byte aligned on function calls. However, memory allocation is only aligned to 4-byte boundaries. To compensate for this, 4 bytes are added to the stack allocation in order to ansure the stack available to the task is at least what was requested from the programmer. This is then aigned to the 8-byte address boundary by clearing the lower three bits of the stack pointer for the task, thus decrementing the stack pointer up to 4 bytes to ensure alignment compliance with the AAPCS.

In mantaining this 8-byte alignment while preserving volatile registers during an interrupt call including context switches, the interrupt decrements the process stack pointer (PSP) by up to 36 bytes (32 if already aligned) when saving context.

The programmer is responsible for allocating at least enough stack space for the interrupt to save context with (9 words). Realistically, most stacks will need to be much larger than this size, particularly if the task is not written in assembly where the user cannot manage stack usage as precisely.

## Argument 3: Arguments

All arguments must be passed through a void pointer. In doing so, arguments are flexibly passed to any task, regardless of function need. In the calling function, the void pointer can be cast to any type needed to be dereferenced.

For functions that may need multiple arguments passed to them, it is reccomended to create a struct with members for needed parameters.

If your function does not need any arguments, this can be left NULL.

## Argument 4: Return Storage Location

A pointer to a void pointer should be passed as the 4th argument. In the event that a task reaches its return, it will return a void pointer for any values needed to be returned. The objective of this argument is to provide a location to store the void pointer returned by the function.

If there is no intention to save a return value, the task should return NULL, and NULL can be provided as this argument.

## Argument 5: Task Properties

Task properties is a series of additional properties passed to `CreateTask`. These properties include:

- Task Priority
- Task Permissions (Not Yet Implemented)
- Suspend State

For convenience, these attributes have been combined into a simple struct type, `TaskProperties`:

```C
typedef struct TaskProperties{
    uint8_t priority;
    uint8_t permissions;
    bool isSuspended;
}TaskProperties;
```

### Priority:
Task priority is broken down into several levels listed from highest priority to lowest: 
	
- `PRIORITY_REALTIME`
- `PRIORITY_VERY_HIGH`
- `PRIORITY_HIGH`
- `PRIORITY_HIGH_NORMAL`
- `PRIORITY_NORMAL`
- `PRIORITY_LOW_NORMAL`
- `PRIORITY_LOW`
- `PRIORITY_IDLE`

Tasks of higher priority, as long as they are not blocked are suspended will always take precedence over tasks with lower priority.

Note that `PRIORITY_IDLE` is only intended to be used by the idle process.

### Permissions:
Tasks are intended to operate in either privileged or unprivileged mode.

This feature is not yet implemented. Should be set to `0`. All tasks will run in privileged mode until implemented.

### Suspended State:
Suspended state determines if a task is active or not at `OsStart`.

- `true` - task is suspended; will not run at start
- `false` - task is not suspended; will run according to task scheduler priorities

## Return Value: TaskHandle

`CreateTask` returns a `TaskHandle` object. This object is simply a void pointer that is used as an identifier for functions that need to use the task.

A `TaskHandle` object should NEVER be attempted to be dereferenced. The data a TaskHandle points to is essential to running the task control block. Modifying this data is considered undefined behavior.

`TaskHandle` objects can be passed to RTOS functions that perform actions to address functionality of a task.

Upon failure, `CreateTask` returns NULL. Tasks should be checked if successfully created.

## Memory Allocated

`CreateTask` performs 3 distinct memory allocations:

1) Allocating the Task Control Block
2) Allocating Stack
3) Allocating Kernel Reference

These three allocations do not need to be contiguous to each other. However it is important to note that Kernel References need to be contiguous to each other. Note the internal structure used:

```C
bool appendTasktoKernel(TaskHandle task){			// adds completed task initialization to Kernel. returns true on successful addition
    if (kernel == NULL){
    	return false;
    }
    AtomicInternalStart();
	TaskHandle * newTaskList = realloc(kernel->tasks, sizeof(TaskHandle) * (kernel->taskCount + 1));
    if (newTaskList == NULL){
        return false;
    }
    kernel->tasks = newTaskList;
    kernel->tasks[kernel->taskCount++] = task;
    AtomicInternalStop();
    return true;
}
```

Here, we can see that kernel tasks are reallocated to be contiguous. If a contiguous block of appropriate size is not available at the current address, the location for the references is moved to a space with a free block. Note that this can cause memory fragmentation. 

Future implementatiions may address this fragmentation, and allocate larger blocks initially to minimize fragmentation.

In addition to this, which theoretically only needs to allocate an additional 4 bytes to the Kernel's task list, The function allocates 26 words (104 bytes) to the Task's Task Control Block.

Lastly, Stack allocation allocates 1 word more than the requested stack size.

The following formula indicates the minimum memory allocation of a successful task creation in bytes, where $x$ represents the number of words to allocate stack size:

$` s(x) = 108 + 4x + 4 `$