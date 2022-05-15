## Memcheck implementation

Today is a fetch-quest coding lab.  You're going to combine the pieces
from several lab into a working trap-based mem-checking system.   It's surprisingly
easy, as long as you don't make foolish mistakes (as I did).


#### Part 0: house-keeping

Last lab, we asked you to copy old files into your new project --- this was good in that
it wouldn't mess with working code, but it's obviously bad in that we have duplicate 
stuff everywhere.  What we should be doing in general is:
  1. Develop pieces in isolation.
  2. Test until we're happy.
  3. Commit the code to `libpi`.

We'll clean up a bit first.

I put the following in libpi:
 - `bit-support.h` into `libpi/libc`.
 - `cpsr-util.h` into `libpi/include`.

Delete these from:
  1. your `part1-single-step` and make sure it compiles.  
  2. your `part2-vm-trap` and make sure it compiles.

#### Part 1: running a routine at user-level: `memcheck_fn`

If you look in `memcheck.h` there's a new function:

    int memcheck_fn(int (*fn)(void));

This will run `fn` in a checked environment that flags if you read or
write unallocated or freed memory in the heap.  For part 1 we are just
going to run it at user level with the heap protected --- as soon as you
get a single fault, disable checking and continue.

The first time it is called (but only the first time!) it should do
any initialization:

   1. Call `memcheck_init` 
   2. Call `single_step_init`.  
   3. Map the heap at  `(uint32_t)pt + OneMB`
       (I think other places should work fine; this just makes it easy for
       us to compare values.)
   4. Map the shadow memory at one MB after the heap (other places work fine too).

Note: the way we are doing this --- by sticking stuff at random constants
---- is really lame and done only for expediency.  The one minor thing
that makes it better is that the MMU code will warn if we conflict and
the MMU itself will throw a fault if we reference memory we did not map.
Longer term we need something less embarassing.

The rest of the routine should:
   1. Turn on mem-checking
   2. call `fn` using your `user_mode_run_fn` routine.
   3. Turn off mem-checking.
   4. Return.


If you look at the tests for part1 you can see they build up slowly: 
   - `part1-test0.c`: this just runs a routine without any checking.
      Make sure this works before you start adding stuff.  Run it first without
      any of the initialization above, then add the init and make sure it still
      works.

   - `part1-test1.c`: this makes sure you are running at user level and
      that you can call a system call.  Just make up a fake one that
      returns 10.

   - `part1-test2.c`: this makes sure you are running at user level and
      that you can take a single domain trap.    For this part you'll have
      to protect the heap and, in the trap handler, change the permissions
      for the tracked domain.  Sadly, it has no internal checking so
      you'll have to look at the output to make sure it makes sense.

#### Part 2: trapping each load and store.

This is the fun part, where we can use the trick we keep discussing

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

For tests:
  - `part2-test0.c`: checks that you can handle multiple traps.  You'll have to 
      look at the output.
  - `part2-test0.c`: checks that you can handle calling your `memcheck_fn` multiple
      times.  You'll have to look at the output.
  - `part2-test2.c`: simple test that shouldn't give any issues --- call `kmalloc`
    and make sure things work.  This checks that you called 
    `kmalloc_init_set_start(heap_start)` to set where the heap started.

There should be more tests, so if you write some that would be great.

#### Part 3: simple shadow

Here you'll do a simple shadow memory.  We'll just allocate a single 4-byte word
to keep things easy.  There are three parts to this:
  1. For each byte of heap memory, we'll have a byte of shadow memory holding
     its state.  You might want to use the partial-definitions in `memcheck-internal.h`
     but you can use you own.  I put the shadow memory right after the heap and its
     the same size (one MB).  Create and map this during your own time initialization.
  2. Implement `memcheck_alloc`, which is a system call that will
     allocate memory, turn checking off so the shadow memory can be
     written, mark its shadow memory as `ALLOCATED`, and then turn
     checking back off.

     How this is used: The exception handler will check if the memory
     being read or written to is `ALLOCATED` and give errors for
     everything else.  It needs to be a system call since eventually we
     will be protecting the shadow memory with its own domain id. 

  3. Implement `memcheck_free`, which checks that the memory is indeed
     allocated and marks the state as `FREED`.  This is a system call
     for the same reasons as `memcheck_alloc`.

***SIMPLIFICATION***: for the moment, just assert that we do a single
four-byte allocation.   We can fold in your checking allocator later ---
we just want to test the shadow memory in the simplest way possible.

Tests:
  - `part3-test0.c`: checks that you can allocate and write to memory without errors.
  - `part3-test1.c`: checks that if you allocate, free and then write to memory
     that you catch it.
