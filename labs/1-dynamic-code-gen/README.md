## Lab: dynamic code generation

***Make sure you read the [PRELAB](./PRELAB.md) for context.***

Generating machine code is fun, kind of sleazy, and most people have
no idea how to do it.  So it makes sense as a lab for our class.

As discussed in the `PRELAB` its used for many things, from the original
VMware, machine simulators (closely related!), JIT compilers you may
well use everyday, and even nested functions in gcc.

A second nice thing about generating machine code is that it really
gives you a feel for how the machine actually operates.  When you do
today's lab you will realize that --- surprisingly --- assembly code is
actually high-level in many ways(!) since the assembler and linker do many
tricky things for you.  (E.g., linking jumps to their destinations.)
You will implement a bunch of these things (next lab).

The theory behind today's lab is that we'll do concrete examples of 
a bunch of different things so that it removes a bunch of the mystery.
Subsequent lab(s?) will go more in depth, where you can build out the things
you do today.

----------------------------------------------------------------------------
### Part 1: reverse engineer instructions (main work today)

The code you'll hack on today is in `code/unix-side`.  Feel free to
refactor it to make it more clean and elegant.    I ran out of time to
iterate over it, unfortunately.
The key files:
  - `check-encodings.c` this has the code you'll write and modify
    for today's lab.  Just start in `main` and build anything it
    needs.

  - `armv6-insts.h`: this has the `enum` and functions needed to encode
    instructions.  You'll build this out.

  - `code-gen.[ch]`: this has the runtime system needed to generate code.
    You'll build this out next time.


If you look at `main` you'll see five parts to build --- each is pretty
simple, but it gives you a feel for how the more general tricks are played:

  0. Define the environment variable `CS240LX_2022_PATH` to be where your
     repo is.  For example:
        
        setenv CS240LX_2022_PATH /home/engler/class/cs240lx-22spr/


  1. Write the code (`insts_emit`) to emit strings of assembly 
     and read in machine code.  You'll call out to the ARM cross compiler
     linker etc on your system.  `make.sh` has some examples.
  2. Use (1) to implement a simple cross-checker that takes a
     machine code value and cross checks it against what is produced
     by the assembler.
  3. Implement a single encoder `arm_add` and use (2) to cross-check it.
  4. Finish building out the code in `derive_op_rrr` to reverse engineer
     three register operations.  It shows how to determine the encoding 
     for `dst` --- you will have to do `src1` and `src2`.  You should
     pull your generated function back in and cross check it.
  5. Do: load, stores, some ALU and jump instructions.

Congratulations, you have the `hello world` version of a bunch of neat
tricks.  We will build these out more next lab and use them.

----------------------------------------------------------------------------
### Part 2: write a `hello world`

Generate a routine to call `printk("hello world\n")` and return.
  - `code/1-hello` has the starter code.

Note: to help figure things out, you should:
  1. Write a C routine to call `printk` statically and return.
  2. Look at the disassembly.
  3. Implement the code to generate those functions.  I would suggest
     by using the cheating approach in `code/examples`.  
  4. To make it really simple, you might want to write exactly those
     values you get from disassembly to make sure everything is working
     as you expect.

You probably want to check your code using something like:

    check_one_inst("b label; label: ", arm_b(0,4));
    check_one_inst("bl label; label: ", arm_bl(0,4));
    check_one_inst("label: b label; ", arm_b(0,0));
    check_one_inst("label: bl label; ", arm_bl(0,0));
    check_one_inst("ldr r0, [pc,#0]", arm_ldr(arm_r0, arm_r15, 0));
    check_one_inst("ldr r0, [pc,#256]", arm_ldr(arm_r0, arm_r15, 256));
    check_one_inst("ldr r0, [pc,#-256]", arm_ldr(arm_r0, arm_r15, -256));

Where my routines are:

    // <src_addr> is where the call is and <target_addr> is where you want
    // to go.
    inline uint32_t arm_b(uint32_t src_addr, uint32_t target_addr);
    inline uint32_t arm_bl(uint32_t src_addr, uint32_t target_addr);

### Part 2: add executable headers in a backwards compatible way.

Even the ability to stick a tiny bit of executable code in a random
place can give you a nice way to solve problems.  For this part, we
use it to solve a nagging problem we had from `cs140e`.

As you might recall, whenever we wanted to add a header to our pi
binaries, we had to hack on the pi-side bootloader code and sometimes
the unix-side code.  Which was annoying.

However, with a simple hack we could have avoided all of this:  if you have
a header for (say) 64 bytes then:
   1. As the first word in the header (which is the first word of the pi binary), 
      put the 32-bit value for a ARMv6 branch (`b`) instruction that jumps 
      over the header.
   2. Add whatever other stuff you want to the header in the other
      60 bytes:
      make sure you don't add more than 64-bytes and that you pad up to
      64 bytes if you do less.

   3. When you run the code, it should work with the old bootloader.
      
      It's neat that the ability to write a single jump instruction
      gives you the ability to add an arbitrary header to code and have
      it work transparently in a backwards-compatible way.

More detailed:
   1. Write a new linker script that modifies `./memmap`  to have a header
      etc.  You should store the string `hello` with a 0 as the first 6 bytes of the 
      after the jump instruction.
   2. Modify `hello.c` to set a pointer to where this string will be in
      memory.  The code should run and print it.
   3. For some quick examples of things you can do in these scripts you may
      want to look at the `memmap.header` linker script from our fuse lab,
      which has example operations that might be useful.  The linker script
      language is pretty bad, so if you get confused, it's their fault, not
      yours --- keep going, try google, etc.     We don't need much for this
      lab's script, so it shouldn't be too bad.
   4. To debug, definitely look at the `hello.list` to see what is going on.

### Part 3: make an interrupt dispatch compiler.

We now do a small, but useful OS-specific hack.  In "real" OSes you
often have an array holding interrupt handlers to call when you get an interrupt.
This lets you dynamically register new handlers, but adds extra overhead
of traversing the array and doing (likely mis-predicted) indirect
function calls.  If you are trying to make very low-latency interrupts
--- as we will need when we start doing trap-based code monitoring ---
then it would be nice to get rid of this overhead.

We will use dynamic code generation to do so.  You should write an
`int_compile` routine that, given a vector of handlers, generates a
single trampoline routine that does hard-coded calls to each of these
in turn.  This will involve:
  1. Saving and restoring the `lr` and doing `bl` as you did in the 
     `hello` example.
  2. As a hack, you pop the `lr` before the last call and do a
     `b` to the last routine rather than a `bl`.

#### Advanced: jump threading 

To make it even faster you can do "jump threading" where instead of the
interrupt routines returning back into the trampoline, they directly
jump to the next one.  For today, we will do this in a very sleazy way,
using self-modifying code.

Assume we want to call:

    int0();
    int1();
    int2();

We:
   1. Iterate over the instructions in `int0` until we find a `bx lr`.
   2. Compute the `b` instruction we would do to jump from that location
      in the code to `int1`.
   3. Overwrite `int0``s `bx lr` with that value.
   4. Do a similar process for `int1`.
   5. Leave `int2` as-is.   

Now, some issues:
  1. We can no longer call `int0`, `int1` and `int2`  because we've modified
     the executable and they now behave differently.
  2. The code might have have multiple `bx lr` calls or might have data
     in it that looked like it when it should not have.
  3. If we use the instruction cache, we better make sure to flush it or at 
     least the address range of the code.

There are hacks to get around (1) and (2) but for expediency we just
declare success, at least for this lab.

#### Extension: making a `curry` routine

One big drawback of C is it's poor support for context and generic arguments.

For example, if we want to pass a routine `fn` to an `apply` routine to 
run on each entry in some data structure:
  1. Any internal state `fn` needs must be explicitly passed.  
  2. Further, unless we want to write an `apply` for each time, we have
     to do some gross hack like passing a `void \*` (see: `qsort` or
     `bsearch`).

So, for example even something as simple as counting the number of
entries less than some threshold becomes a mess:

        struct ctx {
            int thres;  // threshold value;
            int sum;    // number of entries < thres
        };

        // count the number of entries < val
        void smaller_than(void *ctx, const void *elem) {
            struct ctx *c = ctx;
            int e = *(int *)elem;

            if(e < ctx->thres)
                ctx->thres++;
        }


        typedef void (*apply_fn)(void *ctx,  const void *elem);

        // apply <fn> to linked list <l> passing <ctx> in each time.
        void apply(linked_list *l, void *ctx, apply_fn fn);

        ...

        // count the number of entries in <ll> less than <threshold
        int less_than(linked_list *ll, int threshold) {
            struct ctx c = { .thres = 10 };
            apply(ll, &c, smaller_than);
            return c.sum;
        }
        

This is gross.

Intuition: if we generate code at runtime, we can absorb each argument
into a new function pointer.  This means the type disppears.  Which 
makes it all more generic.


To keep it simple:

    1. allocate memory for code and to store the argument.
    2. generate code to load the argument and call the original
       routine.


    int foo(int x);
    int print_msg(const char *msg) {
        printk("%s\n", msg);
    }

    typedef (*curry_fn)(void);

    curry_fn curry(const char *type, ...) {
        p = alloc_mem();
        p[0] = arg;
        code = &p[1];

        va_list args;
        va_start(args, fmt);
        switch(*type) {
        case 'p':
                code[0] = load_uimm32(va_arg(args, void *);
                break;

        case 'i':
                code[0] = load_uimm32(va_arg(args, int);
                break;
        default:
                panic("bad type: <%s>\n", type);
        }
        code[1] = gen_jal(fp);
        code[2] = gen_bx(reg_lr);
        return &code[0];
    }


    curry_fn foo5 = curry("i", foo, 5);
    curry_fn hello = curry("i", bar, 5);

