## Prelab for dynamic code generation

We are going to generate machine code routines at runtime.
This approach can give massive speed wins for a variety of things.  For 
example:

   1.  Instead of interpretation, we can compile the interpeted code
      and then run it directly.   This can easily give more than a 10x
      performance win.  It's used in domains as varied as javascipt 
      JIT backends to how VMware would run operating systems.

   2. Another
      is hardcoding / exploiting "runtime constants" --- values that are
      unknown at compile time, but constant (or sufficiently constant)
      at runtime.

   3. We can also use it to express functionality we cannot in a static language
      (more on this in the lab).

You're likely mostly used to writing code in a high-level language that 
we use a compile to reduce to machine code.  We could do that for this
lab, however compilation is wildly expensive, and so easily wipes out any
possible performance win from generating code at runtime.  Thus, we will
need to figure out ourselves what machine instructions to use and then
how to encode them.  The main thing 
you'll need to figure out to generate useful routines is:

   1. What machine instructions you need to use to accomplish a task.
   2. How to encode them.

The readings below give some background information you can go through.

You should mainly figure out Part 1 and Part 2.  The others are useful
for background, but do not panic if you don't have time to get to them.

### Part 1: Quick and dirty method of seeing encodings.

The easiest quick-and-dirty approach to figure out what instructions to
use and how to encode them is to write C code that looks like that you
want, compile and link it, and disassemble the result.

If you only need an encoding, it's even easier: write the instruction in an
assembly file and compile and link it.

For example, to see how to do an addition, we could do the following:

    # 1. write the C code
    % cat add.c
    int add(int a, int b) { 
        return a+b; 
    }

    # 2. Compile it.
    #   Note: all these flags aren't really needed, but to minimize the change
    #   the compiler sticks some extra stuff in we use them.

    % arm-none-eabi-gcc -O2 -Wall -nostdlib -nostartfiles -ffreestanding  -march=armv6 -std=gnu99 -c add.c 

    # 3. Link it if we need address resolution
    #   Note: we just use <memmap> to stop warnings about no linkage
    % arm-none-eabi-ld add.o -T ./memmap -o add.elf 

    # 4. Disassemble the linked executable has.
    % arm-none-eabi-objdump -d add.elf 
    00008000 <add>:
        8000:   e0800001    add r0, r0, r1
        8004:   e12fff1e    bx  lr

    # 5. To double check that we didn't actually need linking, double check
    # against the unlinked .o
    % arm-none-eabi-objdump -d add.o 
    00000000 <add>:
        0:   e0800001    add r0, r0, r1
        4:   e12fff1e    bx  lr


What did we learn?
  1. ARM has an `add` instruction for addition.  (Not a big surprise.)
  2. You encode `add r0, r0, r1` as the hex value `0xe0800001`.   You should
     look this up in the arm manual, write the code to encode this, and 
     verify you get the same result.

Even easier, if we already know the instruction we want, just put it in
an assembly file and check the encoding directly:

    # 1. write the assembly
    % cat add-asm.s
    add r0, r0, r1

    # 2. use the assembler to encode it as machine code.
    # we just use extra flags to hopefully catch any assembler errors
    % arm-none-eabi-as --warn --fatal-warnings -mcpu=arm1176jzf-s -march=armv6zk add-asm.s -o add-asm.o

    # 3. disassemble it: since there is no branch or address instructions 
    #    just disassemble the .o
    % arm-none-eabi-objdump -d add-asm.o 

    00000000 <.text>:
        0:   e0800001    add r0, r0, r1

This latter approach lets us easily see how to encode instructions that 
are not easily expressed from C code.

### Part 2: ARM machine code encodings

The primary
document you'll need is the arm manual in our top `/docs/` directory so that
you can see (1) the instructions available, and (2) their encodings.

  1. The [shorter instruction chapter (A3)](../../docs/armv6-inst.pdf)
     from `armv6.pdf`  which has encodings and the instruction
     description.  If you need more information you can refer to
     [the longer instruction chapter (A4)](../../docs/armv6-inst.pdf).
     
  2. This [arm architecture talk]](./docs/Arm_EE382N_4.pdf)  is a
     reasonable ARM crash course, which includes an overview of the
     instruction set along with some encodings --- you'll need the later
     to generate machine code.


Optionally, if you'd like some more takes on ARM assembly:
  1. [quick reference](../../docs/arm-asm-quick-ref.pdf) gives a quick
     reference of all the ARM assembly instructions (note: we are not
     going to use Thumb mode).
  2. [ARM Hohl](./docs/subroutines.hohl-arm-asm.pdf) talks about how to pass
     parameters.
  3. The ARM manuals in our top-level `docs` directory have the definitive,
     detailed description of anything you need to know.

### 3. When to use dynamic code generation?

Two fairly concrete papers on dynamic code gen:
  1. [vcode](./docs/cs343-annot-vcode.pdf) which describes a low-level
     macro system I wrote so you could genreate stuff on various
     RISC architectures.  It cost around 10 cycles to generate each
     instruction.

  2. [dpf](./docs/dpf-sigcomm96.pdf): a very fast packet-filter system
     that used dynamic code generation to get rid of interpretation overhead.
     There are some cool tricks (such as compiling a hash table into instructions).


A famous but a bit trickier to get through paper are those on the Synthesis
Kernel by Massalin.  Very cool:
   - [synthesis](./docs/synthesis.pdf) --- there are a set of papers on this
     system


### Part 4: Hacks for reverse engineering instruction encodings.

Encoding binary instructions can be error-prone and is definitely tedious.
As you look at the encodings you will realize two things:
   1. We already have tools that understand them (e.g., the assembler).
   2. While the encodings have weird fields at weird offsets each field 
      is almost always independent of the others.
As a result, you can use a form of linear equation solving to 
efficiently reverse engineer how to encode an instruction.  

For example, assume `add` takes three register fields:

    add rdst, rsrc1, rsrc2

As discussed in the beginning of the PRELAB, we 
can figure out how to encode:
   
    add r0, r0, r0

By simply:
  1. emitting the instruction in a file (possibly with placeholders
     to bracket it)
  2. Run the assembler on it to produce a binary version.
  3. Read in the result of (2): that is the encoding!

To find all register encodings:
  1. Hold `rdst` and `rsrc1` fixed and iterate over all possible
     registers for `rsrc2`.  
  2. Hold `rdst` and `rsrc2` fixed and iterate over all possible
     registers for `rsrc1`.
  3. Hold `rsrc1` and `rsrc1` fixed and iterate over all possible
     registers for `rdst`.
  4. All the bits that never change during (1)---(3) are part of the opcode.

If the instruction fields are idependent, then the bits that change for
any step (the changset) should be disjoint from the changeset of any
other step.  We can check this and panic if it's not true.

A long time ago, we built a tool, Derive, that used this approach
and it worked pretty well.  (It came about because I refused to figure out
how to encode `x86` instructions because they were so disgusting.)  We'll
do a simple version in this lab.  The papers:
   - a [short version](docs/derive-short.pdf)
   - a [longer more short version](docs/derive-usenix.pdf)
