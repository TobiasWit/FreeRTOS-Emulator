# FreeRTOS Emulator

## Exercise 3

### Button explanation

Press button Q to quit.
Press button E to switch to the next state.

State 1 (Exercise 2):
Press button A, B, C or D, increments the number of times these buttons where pressed.
Press the mouse button to reset the count of A, B, C and D.
Move the mouse cursor to move the screen in the direction of the cursor.

State 2 (Exercise 3):
Press button T, increments the number of times button T was pressed (via task notification).
Press button R, increments the number of times button R was pressed (via semaphore).
Count of R and T resets every 15 seconds.
Press button S to stop or restart the seconds counter.

State 3 (Exercise 4):
No user input.

### Exercise questions answerd

#### 3.1 What is the kernel tick?

The kernel tick is a periodic timer interrupt provided by the harware timer. It is a unit of time measurment and it manages the operations of the kernel. The scheduler is called every kernel tick (but the schedular can also be called in between ticks). Although the tick is the smallest unit of measurement in the RTOS, multiple tasks can be executed within a tick.

#### 3.1 What is a tickless kernel?

A tickless kernel reduces the number of timer interrupts, by not generating periodic timer interrupts and only generating timer interrups, when they are needed.

#### 3.2.2.5 Experiment with the task stack size. What happens if it is too low?

If the stack size is too low the program should crash, although that did not happen to me, no matter how low I set the stack size.

#### 4.3 Play around with the priorities.

The order of the output depends on the tasks priorities. In generel, the higher the prioritie, the earlier a task will be executed. If two tasks have the same priority, the order in which the two tasks will be executed, depends on how often the tasks where already executed.