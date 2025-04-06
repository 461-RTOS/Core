# Suspend Functions

Suspend functions control a task's `Suspended` flag. The while the `Suspended` flag is set, a task is ignored by the Task Scheduler. 

*For more information on the `Suspended` state, see [Task Scheduler](./../Task%20Scheduler/Scheduler.md)*

##  ToggleTaskSuspend

The `ToggleTaskSuspend` function is used to toggle a task's `Suspended` flag from active (suspended) to inactive (not suspended) or back.

`ToggleTaskSuspend` takes a `TaskHandle` as a parameter and returns an OS_Status signal:

```C
OS_Status ToggleTaskSuspend(TaskHandle handle);
```

### Argument

`ToggleTaskSuspend` takes a `TaskHandle` as a parameter, to identify the target to have its `Suspended` flag toggled.

### Return Value

`ToggleTaskSuspend` can return two `OS_Status` values:

- `osOK` - Task Suspend Status Successfully Toggled
- `osErrorParameter` - NULL handle passed