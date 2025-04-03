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


- [OsInitialize](Documentation/Functions/OsInitialize.md)
- [CreateTask](Documentation/Functions/CreateTask.md)
- [OsStart and OsStop](Documentation/Functions/OsStart.md)
- [OsDelay](Documentation/Functions/OsDelay.md)
- [Semaphore Functions](Documentation/Functions/Semaphores.md)
- [Mutex Functions](Documentation/Functions/Mutexes.md)
- [Atomic Functions](Documentation/Functions/Atomics.md)


<!-- ## Scheduler -->