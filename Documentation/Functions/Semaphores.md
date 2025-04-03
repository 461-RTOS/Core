# Semaphores

Binary semaphores are implemented into OURTOS as an effective method for Tasks to be signaled by interrupts, or other tasks. The RTOS provides three main functions for Semaphores.

- CreateBinarySemaphore
- SemaphoreRelease
- SemaphoreAcquire


## CreateBinarySemaphore

`CreateBinarySemaphore` is used to initialize a semaphore, for inter-process communication.

The function takes a boolean as a parameter (under a typedef of `SemaphoreAcquiredState`), and returns a `SemaphoreHandle`:

```C
    SemaphoreHandle CreateBinarySemaphore(SemaphoreAcquiredState acquiredState);
```

### Argument: Semaphore Acquired State

The Semaphore Acquired state is a boolean value, intended to be used with two definitions: `SEMAPHORE_STATE_ACQUIRED` as true, and SEMAPHORE_STATE_RELEASED as false:

```C
typedef bool SemaphoreAcquiredState;
#define SEMAPHORE_STATE_ACQUIRED true
#define SEMAPHORE_STATE_RELEASED false
```

This simply sets the default acquired state of the created semaphore. If a semaphore is acquired by default, then a task will need to wait for the semaphore to be released before it can be acquired.

### Return Value: SemaphoreHandle

Much like `CreateTask` with `TaskHandle`, `CreateBinarySemaphore` returns a `SemaphoreHandle` object. This object is also a void pointer that is used as an identifier for functions that need to use the task.

A `SemaphoreHandle` object should NEVER be attempted to be dereferenced. The data a Semaphore Handle points to contains essential data that should not be modified by a task. Modifying this data is considered undefined behavior.

### Memory Allocated

Creating a Semaphore allocates 2 words (8 bytes) on the heap.




## SemaphoreRelease

`SemaphoreRelease` takes a `SemaphoreHandle` as a parameter, and returns an OS_Status signal:

```C
    OS_Status SemaphoreRelease(SemaphoreHandle handle);
```

Releasing a semaphore makes it available for a task to acquire it. If no tasks are waiting, then the semaphore is marked as released, and the first task to reach it can acquire it and continue operation. Other tasks that attempt to acquire the semaphore will be marked as `Blocked` and put to sleep.

### Argument: Semaphore Handle

The handle of a created semaphore can be used to identify the semaphore that should be released.

### Return Value: OS_Status

`SemaphoreRelease` returns an `OS_Status` Signal of one of three values:

- `osOK` - Semaphore successfully released
- `osErrorParameter` - Provided Semaphore Handle is NULL, and not valid.
- `osErrorResource` - The Semaphore is already released, as a binary semaphore, cannot be released more than once



## SemaphoreAcquire

Calling `SemaphoreAcquire` causes a task to attempt to acquire the semaphore whose handle was passed as an argument. If the semaphore is not available, then the task is added to a list of tasks waiting for the semaphore to become available, and is marked as `Blocked`. If the semaphore times out, the task will continue without acquiring the semaphore

`SemaphoreAcquire` takes a `SemaphoreHandle`, and an unsigned 32-bit integer as parameters, and returns an OS_Status signal:

```C
    OS_Status SemaphoreAcquire(SemaphoreHandle handle, uint32_t timeout);
```

### Argument 1: Semaphore Handle

A `SemaphoreHandle` is passed as the first argument to the function to identify which semaphore should be acquired.


### Argument 2: Timeout

The second argument represents the number of ticks that should pass before a timeout occurs for waiting too long and the task resumes operation.

### Return Value: OS_Status

The `SemaphoreAcquire` function can return 4 signals from OS_Status:

- `osOK` - Semaphore successfully acquired
- `osErrorParameter` - A NULL semaphore handle was passed
- `osErrorAllocationFailure` - Failure to allocate memory for Semaphore Task List
- `osErrorTimeout` - Timeout Occurred before semaphore was acquired

### Memory Allocation

Allocates 1 additional word (4 bytes) to the list of tasks attempting to acquire a semaphore. This memory is deallocated from semaphore list as soon as the task acquires it, or times out, but for the duration of the task sleeping, the space remains allocated.

Note: Much like the task list in the kernel, a semaphore has its own task list which must grow and shrink with tasks using the semaphore, if a contiguous block of appropriate size is not available at the current address, the location for the references is moved to a space with a free block. This can cause memory fragmentation.

Future implementatiions may address this fragmentation, and allocate larger blocks initially to minimize fragmentation.