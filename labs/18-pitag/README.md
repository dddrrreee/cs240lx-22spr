### Passing information to pi programs using `PTAGs`

Currently if we want to pass information into our pi programs
we have to either compile it in or use sensors.  This makes it irritating to
do a bunch of things, for example:
  - Passing runtime options to the program using a an 'argv' array 
    passed to  `notmain` (just as on your laptop);
  - Appending a symbol table to our binaries so output and 
    backtraces are nicer;
  - Appending a set of programs to a network bootloader to it can 
    forward these to their destination nodes.


In the interests of time we'll do a hack similar to the `ATAG` method
we looked at all the way back in [140e's lab 10](https://github.com/dddrrreee/cs140e-22win/tree/main/labs/10-low-level) where the loader would attach
information as raw bytes, prefixed with an integer tag and the size of
the record in bytes.  This let you iterate over records and, if you
understood a given record's type, use it.

We'll do the same today, where we use a Unix-side program
(`pitag-linker/`) to concatenate different types of information to a pi
program and override the default `cstart` on the pi side with a fancier
`cstart` that can read these records and store them in a linked list
(see: `reloc-install-pi`).

To make things concrete, I've checked in a complete working system
that understands how to append one pi binary to another and fill in the
right `PTAG` record.  The checked in code uses this to show that the 
position independent bootloader from lab 14 indeed works by using 
`pitag-linker` to construct a series of recursive bootloader
programs that will eventually run `hello.bin`.
This is a pretty good test that the code works.

The key files:
  - `code/cpyjmp.h` --- this has a slightly modified interface from
    lab 14.
  - `ptag-linker/ptag-structs.h` --- these are the structures the
    Unix side writes out and the pi side reads.  Right now there
    is a single type (a program record).
  - `ptag-linker/pitag-linker.c` --- the Unix driver to append a pi
    program to another.  If you have other records you'd add them here.
  - `reloc-install-pi/ptag-cstart.c` --- the trickiest part of the code.
    This overrides the default `cstart` (in `libpi`) and constructs
    a linked list of `ptag` records (if there are any). 

    The main non-obvious tricky thing is that we *must* move the pitag
    region before zeroing out the `bss` (see `cstart`) or there is a
    good chance the `PTAG` region will get smashed.  A secondary tricky
    thing is that moving the `PTAG` region occurs before the `bss`
    has been initialized, so global / static zero-initialized variables 
    have not been.

  - `reloc-install-pi/reloc-bootloader.c` --- trivial driver that uses
    the `ptag` interface to get the first record and jump to its code.

What to do:
 - look through the code and make sure you understand it!  It shouldn't
   be conceptually deep: we just happened a record to a binary.  


 - Replace the `staff-cpyjmp.o` with yours from lab 14.  This will require
   writing a tiny routine that tells the code how large the copy routine
   is --- to make things more robust we have switched to using `kmalloc`
   rather than writing things to a magic address since the whole point of
   `kmalloc` is to find an unused portion of memory.  First make sure that
   `code` runs.  Then change the `ptag-linker` `Makefile` to use your
   `cpyjmp.o` rather than our `staff-cpyjmp.o`.

   Make sure everything still runs as before!

 - Do something useful --- e.g. add a new `PTAG` record, add more 
   checksums to the `PTAG` region (e.g., over the whole thing) so 
   that we are more protected from unexpected corruption.  What
   we are doing is not deep, but if something goes wrong it will
   often do so erratically (hard to test for) and in subtle ways
   (e.g., because we corrupt some code).

   There's a bunch of `EXTENSION` comments of possible things to do
   or do your own.  