# Mutexes

Mutexes are implemented into OURTOS as an effective method for Tasks to be avoid race conditions when using a shared resource.

- CreateMutex
- MutexRelease
- MutexAcquire


## CreateMutex

`CreateMutex` is used to initialize a Mutex, for inter-process communication. It takes no arguments, and returns a handle to the Mutex.

```C
    MutexHandle CreateMutex(void);
```

### Return Value: MutexHandle

Much like `CreateTask` with `TaskHandle`, `CreateMutex` returns a `MutexHandle` object. This object is also a void pointer that is used as an identifier for functions that need to use the task.

A `MutexHandle` object should NEVER be attempted to be dereferenced. The data a Mutex Handle points to contains essential data that should not be modified by a task. Modifying this data is considered undefined behavior.

### Memory Allocated

Creating a Mutex allocates 3 words (4 bytes) on the heap.




## MutexRelease

`MutexRelease` takes a `MutexHandle` as a parameter, and returns an OS_Status signal:

```C
    OS_Status MutexRelease(MutexHandle handle);
```

Releasing a mutex makes it available for a task to acquire it. If no tasks are waiting, then the mutex is marked as released, and the first task to reach it can acquire it and continue operation. Other tasks that attempt to acquire the Mutex will be marked as `Blocked` and put to sleep.

A mutex can ONLY be released by the task that acquired it.

### Argument: Mutex Handle

The handle of a created Mutex can be used to identify the Mutex that should be released.

### Return Value: OS_Status

`MutexRelease` returns an `OS_Status` Signal of one of four values:

- `osOK` - Mutex successfully released
- `osErrorParameter` - Provided Mutex Handle is NULL, and not valid
- `osErrorResource` - The Mutex is already released cannot be released more than once
- `osOwnershipFailure` - A task that did not acquire the mutex attempted to release it



## MutexAcquire

Calling `MutexAcquire` causes a task to attempt to acquire the Mutex whose handle was passed as an argument. If the Mutex is not available, then the task is added to a list of tasks waiting for the Mutex to become available, and is marked as `Blocked`. If the Mutex times out, the task will continue without acquiring the Mutex

`MutexAcquire` takes a `MutexHandle`, and an unsigned 32-bit integer as parameters, and returns an OS_Status signal:

```C
    OS_Status MutexAcquire(MutexHandle handle, uint32_t timeout);
```

### Argument 1: Mutex Handle

A `MutexHandle` is passed as the first argument to the function to identify which Mutex should be acquired.


### Argument 2: Timeout

The second argument represents the number of ticks that should pass before a timeout occurs for waiting too long and the task resumes operation.

### Return Value: OS_Status

The `MutexAcquire` function can return 4 signals from OS_Status:

- `osOK` - Mutex successfully acquired
- `osErrorParameter` - A NULL Mutex handle was passed
- `osErrorAllocationFailure` - Failure to allocate memory for Mutex Task List
- `osErrorTimeout` - Timeout Occurred before Mutex was acquired

### Memory Allocation

Allocates 1 additional word (4 bytes) to the list of tasks attempting to acquire a Mutex. This memory is deallocated from Mutex list as soon as the task acquires it, or times out, but for the duration of the task sleeping, the space remains allocated.

Note: Much like the task list in the kernel, a Mutex has its own task list which must grow and shrink with tasks using the Mutex, if a contiguous block of appropriate size is not available at the current address, the location for the references is moved to a space with a free block. This can cause memory fragmentation.

Future implementatiions may address this fragmentation, and allocate larger blocks initially to minimize fragmentation.