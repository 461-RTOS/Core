# OsDelay

The `OsDelay` function puts the current task to sleep for a number of ticks set by the programmer. The task is marked as `Waiting` until the tick counter is complete.

The `OsDelay` function takes an unsigned 32-bit integer as a parameter to specify the number of ticks to delay operation:

```C
    void OsDelay(uint32_t ticks);
```

During a task's delay period where it is marked as `Waiting`, the task can be preempted by other tasks. The same priority rules still apply for all other tasks, meaning tasks of higher priority will always take precedence over lower priority tasks.