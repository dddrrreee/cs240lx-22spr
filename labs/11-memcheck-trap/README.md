## Simple memory tracing and memory checking.
 
Today is a fetch-quest coding lab.  You're going to combine the pieces
from several lab into a working trap-based mem-checking system.   It's
surprisingly easy, as long as you don't make foolish mistakes (as I did).

High bit:
  1. We give you simple a complete, working but simple trap-based
     memory tracing system (`memtrace.c`) that calls a client supplied
     routine on every load or store.  The client can check properties
     or even modify what occurs.  It works by using domain permissions
     to trap every load/store to a desired region of memory and then
     single stepping to execute it.

  2. We also give you a simple memory checker (`checker-purify.c`)
     that uses (1) to check if each load or store is to valid, allocated
     memory.

  3. The lab is a "choose your own adventure" where you take this working
     system and make it interesting: faster (e.g., using shadow memory),
     replace our code with yours, write new checkers, etc.

What to do:

  1. Make sure the code works: 

            % cd code
            % make check

    should should pass all tests (this can take awhile).

  2. Look over the checker (`checker-purify.c`) and the tracing code
     (`memtrace.c`).    These are pretty short and have comments so
     hopefully not too bad.  They call various helper routines, but most
     of these you have already built (e.g., the pinned VM from lab 7,
     the debugging hardware from lab 9 in 140e).

  3. Likely good starting point: Add shadow memory to the purify code
     (make a copy first!) and measure how much it speeds things up.

Should be relatively clean lab but has enough material you could work on it
all summer if it's interesting (or in cs340lx).

-----------------------------------------------------------------------------
#### Background: the big idea.

If you can see every load or store you can check many things: race
conditions, memory corruption, use of tainted values, etc.


Unfortunately, the traditional way tools such as valgrind and pin trace
these is very complicated.  They use dynamic translations (for code 
discovery) and then some form of instruction emulation to determine
an instruction's effects.  It's hard to do the first in few lines of
code and as you recall from lab `9-memcheck-stat`  figuring out ARM
memory operations is a significant pain.

We'll use domain trapping to trace all memory operations to a 
contiguous region of memory and then single stepping to have the
CPU do the operation for us:

   1. To trace a region: give it a unique domain and then remove
      permissions for that domain.  Everything will run as before, until
      there is a load or store to the range, upon which we get a fault.

      Recall: ARM has 16 domains Each page is tagged with a domain.
      You can switch domain permissions with a single CP15 register write
      (fast).

   2. We then call a client handler on the trapped address; the client
      code can then do whatever checking or modification it wnats.

   3. After the code returns: we need to emulate the trapped instruction.
      We can't just jump back or we'd trap again.  On a simple
      architecture (e.g., riscv) we could just emulate the instruction.
      However, we have ARM.  Fortunately we have single stepping: set
      a mismatch breakpoint, disable trapping, jump back,  run that one
      instruction, get a prefetch abort fault, and then reenable trapping
      and resume until the next load or store.

      This is what we do.
    
      The main weird issue we have to handle is that single stepping only
      works for user mode.  Thus, we have to switch modes from the kernel
      (`SUPER` mode) to user mode.   We can do this for memory because
      they are not privileged instructions.  But we do have to migrate
      shadow registers to user from super and back.

#### Reiterate: trapping each load and store.

Basic idea:
  1. Remove permissions from the heap memory.
  2. Run the code at user level.
  3. If it doesn't load or store to the heap (does ALU operations, or memory operations
     elsewhere) you will get no traps and run at full speed (very different from 
     valgrind!).
  4. If it does load or store the heap, you will get a domain section trap.

How to handle a domain section trap:
  1. Change permissions for the heap domain id so the code can read or write it (as 
     in part 1).  
  2. Before you jump back: Set a single step mismatch on the exception program counter.
     Now, when you jump, it will *only* run that one single instruction and then 
     give a single-step mismatch exception.
  3. In the mismatch exception handle, remove access to the domain id (by calling
     `memcheck_trap_enable` and disable the mismatch breakpoint.  Now, when you 
     jump back, the code will continue as before.


-------------------------------------------------------------------------------
#### Option: add simple shadow to `check-purify.c`

Here you'll do a simple shadow memory.  We'll just allocate a single 4-byte word
to keep things easy.  There are three parts to this:
  1. For each byte of heap memory, we'll have a byte of shadow memory holding
     its state.  I put the shadow memory in the second half of the heap
     Create and map this during your own time initialization.

  2. In `purify_alloc`, turn checking off so the shadow memory can be
     written, mark its shadow memory as `ALLOCATED`, and then turn
     checking back off.

     How this is used: The exception handler will check if the memory
     being read or written to is `ALLOCATED` and give errors for
     everything else.  It needs to be a system call since eventually we
     will be protecting the shadow memory with its own domain id. 

  3. In `purify_free` mark the state as `FREED`. 

  4. Measure how much things get sped up.  (For the slow test it should
     be reasonable).

  5. The `tests/*-purify-*c` test memory checking.

-------------------------------------------------------------------------------
#### Option: replace a bunch of our `.o` files.

We use a bunch of code from old labs.  You should already have versions,
so can start dropping in yours instead of ours.  

  - One issue: the `pinned-vm.h` lab had wrong enums.  You should use the 
    headers from libpi.


-------------------------------------------------------------------------------
#### Option: write the `switch_to` routines

To resume execution we need a `switch_to` implementation that can jump
either to user or to kernel space.  
  - user mode: you can use the `^` operator to modify the user shadow.
  - privileged: you have to switch to the mode and write to the registers
    manually.

-------------------------------------------------------------------------------
#### Option: write new checkers.

Hopefully it's pretty easy to write new checkers.  Some simple ones:
  - check if a routine returns a pointer to its stack (so subsequent reads
    or writes will be beyond the end of the stack pointer);
  - or if it blows out its thread stack (similar);
  - or if it uses uninitialized stack memory (you can random fill on
    each procedure call).
