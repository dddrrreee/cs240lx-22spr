# Tricks for Single Stepping
We will do two tricks that we have talked about in class several times:
1. Single stepping through super level code.
2. Pressing a key on your keyboard and printing a backtrace.

## Part 1: Single Stepping through super level code.
To build the code, you will need to add ```kernel-debug.S``` as a ```SRC``` in your libpi. Please also copy your ```armv6-debug-impl.h``` from 140e where the single stepping functions implemented into the part-1 folder. If people in lab need an implementation, let me know.

From CS140e, we know that we can only single step through user level code. Therefore, it is not possible for us to single step through instructions that cannot be run in user mode. 

For example, ```mcr``` and ```mrc``` throw an undefined instruction fault when run in user mode.
To successfully run these instructions, we emulate these instructions in the undefined instruction handler, and then resume from the next instruction. We do not want to resume from the instruction that we faulted on because it would just throw another undefined instruction fault.
To do this:
1. First, we figure out which ARM register is involved in the ```mrc``` or  ```mcr``` instruction.
2. We read the value of that register. In this implementation, in addition to the ```pc```, my undefined instruction handler also takes a pointer to the ```sp``` as an argument. The registers are stored at offsets from ```sp```. So, figuring out the correct value is a matter of reading from the correct offset.
3. Now, generate a trampoline that runs the ```mrc``` or  ```mcr``` instruction with ```r0``` as the ARM register. We use ```r0``` because by standardizing the register we want to store or load from, we do not need case statements that generate the correct trampoline based on the original ARM register. This should be 2 instructions.
4. If you cast this trampoline as a ```uint32_t (*fp)(uint32_t)``` you can load the value of the original ARM register into ```r0``` when you call ```fp```. Run ```fp``` with the value of the original ARM register passed in.
5. If it was a ```mrc``` instruction, we need to store the value that was returned into the original register. So, we need to write at the correct offset from ```sp```. After returning from the handler, we pop these registers, and the right register will have the updated value.

    If it was a ```mcr``` instruction, we need not store the value that was returned into the original register. In this case, the value that is returned is the original value anyway. So, we can store the same value at the correct offset here too. No need for casing!


Your goal is to do step 3 in ```test.c```. It should take 2-4 lines of code.
## Part 2: Interrupting on UART RX.

First, in ```pi-echo-boot``` build a new ```my-install``` that calls ```pi-echo``` instead of ```pi-cat```. ```pi-echo``` sends any character you type in your terminal to the pi. 
 
Now, we will add an interrupt on UART RX. This is basically a fetch quest, and you should look in the ```uart-int.c``` to implement the relevant functions.
 
Finally, you will need to implement ```interrupt_vector``` in ```test.c``` to pull the data from the UART FIFO and print a backtrace. To print the backtrace, call ```backtrace()```.
 
The complete implementation should not exceed 20 lines at most. There are hints in the comments which basically tell you where to look in the BCM Manual. The manual has many helpful annotations by Dawson that point out errata. The one errata it is missing is the fact that the RX and TX bits for interrupts are flipped in ```AUX_MU_IER_REG```.
 
 
## Optional: Implement interrupts for UART TX.
 
From my experience, it seems like the UART TX interrupt keeps triggering if the TX fifo is empty. You'll want to switch off TX interrupts if you do not have any data to transmit.
 