### Simple memory protection.

I'm not sure how far this aspiration will survive, but I'm hoping
whenever someone hits an interesting bug in class, the next lab we can
build a tool or method that would have prevented it.  This lab is from
two interesting bugs that hit Evan and one that hit Igor.  These were
very tricky to figure out, examples of more general bugs that can hit
the rest of us, and with the right infrastructure, easy to find.

The first we'll handle is that 

What we need:
    1. Memory protection to catch Evan's bug.  This will make virtual
       memory very simple
       exploiting a common (universal?) feature of TLB hardware ---
       pinning TLB entries.  The ARM has 8 pinnable TLB entries (each
       up to 4MB in size).  Since we have small processes we can do
       virtual memory (and thus memory protection) without the need
       for understdanding or having page tables by simply pinning the
       mappings we need.

       I haven't seen anyone using this trick in this way, so we'll be
       ahead of the curve.  

       In retrospect we should have done this in cs140e as our first
       step for making virtual memory since its so simple.

       You can apply it in other ways, for example
       having 10-100s of thousands of processes by eliminating the need
       for a page table by simply pinning entries on context switch.

    2. [next time] Linker script hacks: currently our processes are squished together
       in one single page: we want to seperate the code and read-only data
       from writeable data on different pages.  You'll do so by modifying
       the class linker script.  You'll then modify the "loader" code in
       `libpi/staff-src/cstart.c` to lay things out as needed.

    3. Watchpoints to catch Igor's bug.

All of these are examples of general things that are useful to know.
In particular one of the big things we never covered is how to do
linker scripts.  Also we have not done a ton of binary manipulation.
This lab will make those concepts more clear.

At this point you'll have a process structure that will:
   1. Detect wild reads and writes of unmapped memory.   This will
      prevent many bugs that come up currently and in cs140e.
   2. Detect writes to the read-only segment: this would have caught
      Zach's bug.
   3. Make reads of null pointer's fail.


---------------------------------------------------------------------
#### Catch reads and writes of illegal memory.

Add these to your `libpi/Makefile`:

    STAFF_OBJS += staff-objs/interrupts-asm.o 
    STAFF_OBJS += staff-objs/interrupts-vec-asm.o

For this step, we'll set up a simple virtual address space that includes
code, data, stack, heap, and GPIO memory using 1MB pages (in arm parlance
"segments") and leaves everything else unmapped.  Any read or write to
unmmaped will cause a fault.

This is a good first step and is simple enough we should probably always
have it.  Because of how the current linker script lays out code and
data we won't be to detect writes to the code segment --- the next step
handles this.

What to do today:
  - Read the pages: 3-149--- 3-152 and 3-80 --- 3-82.
  - Implement `pin_mmu_on` to pin all the memory needed and turn the MMU on.
    (you can adapt your 140E code).


If you want, you can ignore our starter code and write all that from scratch.
If you want to use our stuff, there's a few helpers you implement.


Where to look:
  - `pinnned-vm.c`: all your code goes here.  The main routine is `pin_mmu_sec`
    which pins a section and `tlb_contains_va` which looks up the virtual address.
  - `tests/0-test3.c`: the one test.

Given code:
  - `pinned-vm-asm.h`: this has prototypes for the assembly.
  - `procmap.h`: a simple map of the address space: we'll map this.

---------------------------------------------------------------------
#### [for next time] Prevent null pointers.

If you look at the linker script and any `.list` file you'll see that
we link the code starting at `0x8000`.   Since null (`0`) is within
1MB of this we can't catch null pointer reads and writes using the
current linker script and 1MB pages.

Our first hack is to use 4K pages for this first few mappings.
This lets us leave the 0 page unmapped and catch faults.

---------------------------------------------------------------------
#### [for next time] Rewrite the linker script to put code and data on different pages.

Using small pages requires the least amount of changes but  uses 
lot of extra entries (we only have 8).

In addition, tf you look at the linker script and any `.list` file
you'll see that the code and data are smooshed together in the addrss
space and reside on the same 4K portion of memory.  Because everything
on one page has the same protection this means we can't give code and
data different protections.

For this part you'll rewrite the linker script to put code and read-only
data on one page and start the rest of the data on another.  As a result,
we can prevent modification of code.  However, this will require modifying
`cstart.c` to understand how to lay out the new binary.

---------------------------------------------------------------------
#### Rewrite the linker script to only need 1MB segments

---------------------------------------------------------------------
#### Extension:  rewrite 140e `fake-os` to use tiny processes

You should be able to have 4k processes and have 100,000s or processes.
This should be a world record.

---------------------------------------------------------------------
#### Postscript

At this point you should have a much more grounded understanding of:
  1. How exactly processes get started;
  2. How processes are laid out in the first place;
  3. What linker scripts do;
  4. How `cstart` works.
  5. How do do quick and dirty virtual memory.
