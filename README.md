Functons for User to Add: (These are all functions the user has access to)
* CreateTask - Create task with properties and add to task control block. Recieves func pointer(Task to execute), stack size(32-bit word, int), void point(arguments for the pointer), pointer to void point(For return values), TaskProperties(properties object)
* ChangeTaskProperties - Edit mutable properties of task. sent TaskHandle(the task to edit), TaskProperties(new properties)
* ToggleTaskSuspend - Toggles task suspension. Recieves TaskHandle(task to toggle)
* IsTaskSuspended - Boolean returns True if task is suspended. Recieves TaskHandle(task to check)
* DeleteTask - Removes task from task control block. Recieves TaskHandle(task to delete)
* OsInitialize - Initialize timer, setup task control block structures, setup error handler. Recieves timer from user
* OsStart - Starts timer, activate task scheduler, gives control to RTOS
* OsStop - Turns off RTOS, deacivates task scheduler, turns off timer
* OsKill - Deletes task control, unitialize timer

Helper Functions: (Functions for us to help make the above functions possible)
* TaskControlInit - Initialize data structure that task table will use to switch tasks. Returns True when finished(For troubleshooting)
* AllocateStack - Makes space on stack for a task. Takes in stack size to be allocated(Unsigned Int). Returns pointer to top of stack for the task(This stack section holds the task information)
* CreateEntryPoint - Makes a pointer to the start of a function. Stored in task handle struct.
* 
