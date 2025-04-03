# Semaphoress

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

### Return Value

`SemaphoreRelease` returns an `OS_Status` Signal of one of three values:

- `osOK` - Semaphore successfully released
- `osErrorParameter` - Provided Semaphore Handle is NULL, and not valid.
- `osErrorResource` - The Semaphore is already released, as a binary semaphore, cannot be released more than once



## SemaphoreAcquire

`SemaphoreAcquire` takes a `SemaphoreHandle` as a parameter, and returns an OS_Status signal:

```C
    OS_Status SemaphoreRelease(SemaphoreHandle handle);
```