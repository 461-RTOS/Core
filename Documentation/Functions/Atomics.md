# Atomic Functions

Critical Sections of Code may mandate that certain Interrupts or Context Switches not interfere with an operation. `AtomicStart` and `AtomicStop` are intended to Block Interrupts and Context Switches during critical sections.

## AtomicStart

The `AtomicStart` function is used to begin an Atomic Block by setting a new Base Priority. It takes one parameter to define the block level, and returns the status of the block.

```C
    AtomicStatus AtomicStart(AtomicBlockLevel level);
```

Notice 1: Atomic Blocks will prevent context switching, and are not automatically ended. It is the Responsibility of the programmer to manually end an atomic block. Atomic Blocks must ALWAYS be terminated with `AtomicStop`

Notice 2: `AtomicStart` is not designed for nested Atomicity (an atomic block cannot be within another atomic block).


### Argument: Atomic Block Level

`AtomicStart` can have three values passed to it, which each reference a level of blocking:

- `ATOMIC_BLOCK_CONTEXT_SWITCH` - Blocks Context Switches (Managed by `PendSV` Exception Handler) but still allows `SysTick` to increment ticks and schedule future tasks
- `ATOMIC_BLOCK_SYSTICK` - Blocks `SysTick` incrementing and Task Scheduler
- `ATOMIC_BLOCK_ALL` - Blocks All Interrupts except those with Priority `0`

These levels are used to block various Priorities, and are defined based on the priority of the interrupts they are intended to block:

```C
// Enum to be passed to AtomicStart based on needs of Critical Section
typedef enum AtomicBlockLevel{
	ATOMIC_BLOCK_CONTEXT_SWITCH = (0x0F << (8U - __NVIC_PRIO_BITS)),		// Blocks Context Switcher (PendSV)
	ATOMIC_BLOCK_SYSTICK = (0x0E << (8U - __NVIC_PRIO_BITS)),				// Blocks SysTick and Task Scheduler (And Context Switcher)
	ATOMIC_BLOCK_ALL = (0x01 << (8U - __NVIC_PRIO_BITS)),					// Blocks all interrupts except priority 0
}AtomicBlockLevel;
```
### Return Value:

`AtomicStart` Returns the status of the Atomic Block, which can be one of two values:

- `ATOMIC_OK` - Atomic Block Successfully Started.
- `ATOMIC_NESTED_ERROR` - Atomic Block Attempted within an existing Atomic Block. No new Block Started.




## AtomicStop

The `AtomicStop` function is used to end an Atomic Block, restoring the previous Base Priority. It takes no parameters, and returns the status of the block:

```C
    AtomicStatus AtomicStop(void);
```

Notice 1: Atomic Blocks will prevent context switching, and are not automatically ended. It is the Responsibility of the programmer to manually end an atomic block. Atomic Blocks started with `AtomicStart` must ALWAYS be terminated with `AtomicStop`

### Return Value:

`AtomicStop` Returns the status of the Atomic Block, which can be one of two values:

- `ATOMIC_OK` - Atomic Block Successfully Ended.
- `ATOMIC_FLOW_ERROR` - Atomic Block Attempted to be stopped without an active Atomic Block