## Statistical valgrind

***Make sure you do the [PRELAB](PRELAB.md)!***

Over the next few labs we will build a simple memory checking tool that
can check if an executing program performs a read or write to invalid
memory.  Typically such tools (Valgrind, Purify) are incredibly complex
and large (100K lines of code or more).  Most of their complexity comes
from the fact you cannot reliably statically figure out what is code and
what is data in a program.  As a result, they have to do dynamic binary
instrumentation, which requires the ability to parse and (much harder)
correctly specify the semantics of all binary instructions.

In constrast we will use a new trick to build one that is simple, and
a few hundred lines.

Our tool works as follows:
  1. We discover code not by dynamically tracing the program, but instead
     by using timer interrupts to statistically sample the program binary.
     By definition: any address we interrupt contains an instruction
     (since it was being executed).

  2. We decode the interrupted instruction to see if it is a load or 
     a store.  For each load or store we discover we  replace it with
     a branch instruction that jumps to 
     custom checking routine for that specific instruction address.
     Because the jump is the same size as the load or store, the program
     binary does not change size ("dialate") and so we do not have to
     patch program addresses (infeasible if you can't reliably classify
     code and data).  Because the trampoline is customized to that
     specific instruction, we can hardcode any knowledge it needs,
     including the return address it needs to jump back to.

  3. In each trampoline call, we check the state of each memory location
     using the common method of "shadow memory" --- all bytes the program
     can access have a corresponding shadow location that tracks their
     state (allocated, freed, invalid).  Shadow memory is allocated at
     a specific, known offset from the proram's memory so that it is
     fast and simple to look up the state of a memory location given
     just the address.  (A few bounds checks and then an addition.)

   4. The end result: you should be able to take unaltered binaries,
      and without MMU support, detect a variety of memory errors 
      automatically.

There is a lot going on in these set of labs, so we do a very stripped down,
minimum viable product.  It's not general, it's not rebust, but it will
have `hello world` level examples of the key pieces so that you have
something to think about, which will help when we build
these out later.

The tool has four pieces:
   1. Make a simple redzone memory allocator. This will
      catch some number of memory corruptions, and will help prevent us
      from missing if a pointer jumps from one object to another.
   2. Make a simple shadow memory implementation that tracks what
      chunks of memory are allocated, freed, invalid, or read only.
      This will allow us to take any address in the program and figure out
      if it is legal.
   3. Use timer interrupts to make a sampling routine that examines the interrupted
      instruction and, if it is a load or sore, uses the shadow memory in (2)
      to determine if the access is legal.
   4. To make sampling more thorough: when you discover a memory location
      in step (3) instead of checking and returning, we instead
      rewrite the instruction to call a trampoline that will check it.
      This means that when we interrupt a load or store once, the check
      will be performed for all future executions.  I.e., it is sticky,
      or persistent.

Today we focus on the first part, making a redzone allocator.  As a
warmup for shadow memory you will also build a simple garbage collector
using a cute hack from Hans Boehm.

Checkoff:
  1. Showing you find the errors in the checked in programs.
  2. Adding a new buggy program to Peter's contrib repository so that 
     we can extend things.

### Part 0: some new functionality.

I put a new version of libpi in the top of our repository --- it was
getting a bit weird to modify the cs140e code.   You can put any code
you want to use in `libpi/src` and modify `put-your-src-here.mk`.
    - The example in `examples/hello` should run if you compile it.

I also set it up so you can use floating point.  
    - The example in `examples/float` should run if you compile it.

Its a hack where we compile `libpi` in two different ways (with float
and without).   The linker should give you an error if you link against
the wrong one.

### Part 1: redzone allocation.

If you look in the `code` directory, there is a simple bit of starter
code.  The instructions are in `ckalloc.c` and `ckalloc.h`.  You will
write your allocator and, because we all use the same initial heap and
same conceptual approach you should get the same tracing output that we
can cross-check.

Start with my test programs (there is only one there now, I need to check
in others) and then please write at least one of your own to test that
the allocator finds memory is should.

### Part 2: garbage collection.

***NOTE: we moved this to next lab***
