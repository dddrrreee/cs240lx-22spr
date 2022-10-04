## Building a relocatable bootloader.

Today we're going to build "bootloader" code that moves itself out of
the way of the code it copies so that we don't have to make an external mess
of special Makefiles, relinking, etc.  (As we had to do in 140e.)

Recall the bootloading problem:
   1. We are given a buffer of client code that needs to be placed at address 
      `[addr,addr+nbytes)`.
   2. Of course, in order to do so, the executable code to copy this
      client code but be somewhere.
   3. If the copy code overlaps in any way with the address range 
      `[addr,addr+nbytes)` this can't work.

As a hack, our original bootloader satisfied condition (3) by linking
itself as at a fixed "out of the way" location (based on `dwelch67`).
However, this hack requires all external software, linker scripts, etc
know where this special bootloader address location was so they could
avoid it which is fragile and also a hassle in practice in a variety of
low-level ways.

One irritating result of this approach is that it made bootloader
development much slower than for regular pi programs since we couldn't
bootload our bootloader and instead had to constantly pull the SD card
out, copy the bootloader over it, put back in, etc.  Today's lab will
fix this issue, which will make our subsequent network bootloader easier
to build.

What we will do:
  1. Make a dynamic bootloader that can move its code copying loop out
     of the way of `[addr,addr+nbytes)`.
  2. This actually pretty simple, but is a great teaching example to
     think about code, data, and binary-level things.
  3. Its also a handy piece of code that you can use in other bootstrapping 
     contexts.  (e.g., when kernel starts running in low physical addresses
     and then wants to continue in the high ones after virtual memory enabled).

Strategy:
 1. Implement a routine `cpyjmp` that can copy and jump to a RPI binary where
    `cpyjmp` is implemented entirely with position independent code and so
     can be copied out of the way of wherever the incoming code wants to 
     go.  "Position independent" --- e.g., only uses internal relative jumps 
     for control flow so if you copy the code it will still work.

     If you recall we had a similar requirement with the interrupt handlers.
     In that case we used absolute jumps --- which can also be copied anywhere.

 2. How to make sure the code is position independent?  The tedious way is to 
    write it in assembly.  But this is a pain.   We do a hack instead:
    simply instantiate the routine twice and do a byte comparison 
    to check that there is no difference b/n the two instances.  This
    guarantees that it's position independent on each compilation in 
    a roughly architecture independent way.

    Note 1: this check depends on gcc emitting routines in the order we write 
    them.  We compile with `-fno-toplevel-reorder` for this.
   
    Note 2: you cannot call external routines from the copy routine once
    it's been copied *unless these are absolute jumps* (e.g., using a function
    pointer or BRANCHTO) since external relative jumps will go to the wrong
    place after you move the code.  (if this matters, its not hard to pass in
    a vector of of routine pointers or even use swi instructions as a form
    of dynamic linking :).

---------------------------------------------------------------------------
### Implementation: `cpyjmp.c`

We currently have one test in `0-test-reloc.c` that copies itself over
itself `N` times using `cpyjmp_default`.  You should be able to compile
and run the checked in code and get something like:

    cnt=2: about to copy our code over itself: nbytes=4592
    cnt=3: about to copy our code over itself: nbytes=4592
    cnt=4: about to copy our code over itself: nbytes=4592
    cnt=5: about to copy our code over itself: nbytes=4592
    cnt=6: about to copy our code over itself: nbytes=4592
    cnt=7: about to copy our code over itself: nbytes=4592
    cnt=8: about to copy our code over itself: nbytes=4592
    cnt=9: about to copy our code over itself: nbytes=4592
    cnt=10: about to copy our code over itself: nbytes=4592
    stopping iteration cnt=11

Note: The test does a hack where it relies on the counter variable variable
`cnt` being in data segment (not the bss) so that it doesn't get reset on
each iteration.  It's useful understanding this :).

To write your own code: change the `Makefile` to:

    SRC := cpyjmp.c
    # STAFF_OBJS := staff-cpyjmp.o

which will then use the file `cpyjmp.c` which you implement.

There are three routines to build:

   - `cpyjmp_internal(uint32_t code_dst, const void *code, unsigned
     nbytes)`: the copy and jump routine.  It copies `nbytes` of `code`
     to `code_dst` and then jumps to `code_dst`.  

     This should look like a simple `memcpy` and then a cast of `code_dst`
     to a function pointer that the code calls.  

     The routine should generate position independent code and not call
     out to any routine.  (Look at the listing to see that the code seems
     ok to get an intuition).  It is an inline routine so we can inline
     it twice to check position independence.

   - `cpyjmp_check`: this has some cleverness: you should check that two
     instantiations of `cpyjmp_internal` are (1) the same size and (2) 
     identical.     

      You will make two routines that call `cpyjmp_internal`.  You can 
      determine their size by declaring an empty, non-static routine
      after each and using the `gcc` option `-fno-toplevel-reorder` to 
      prevent `gcc` reordering.

   - `cpyjmp_relocate(uint32_t addr)`: copies the `cpyjmp` routine to address
     `addr` --- `addr` should not overlap with the eventual code we copy 
     in any way.  

      If you look in the header file `cpyjmp.h` you can see our hack
      for figuring a free part of the address space is to copy the 
      routine above the libpi constant `HIGHEST_USED_ADDR` which contains
      the largest address libpi uses.

After you build these, the tests should work.   You now have a routine
that can copy code to any location in a running pi program without the
need for static setup.
