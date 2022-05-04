## Doing checks using interrupts (and binary rewriting)

Today we're going to build a simple interrupt-based checker for finding
heap corruption.  We'll extend it next time so it gives you closer to
Purify-functionality, but today we build toy examples of most of the
pieces we'll need and then build out key pieces.

Today:
  1. Look over the interrupt code (`timer-int`) --- it's from last cs140e, so 
     should be familiar to most of you.  Note that some of the code it uses
     is in `libpi/staff-src` and `libpi/include` --- check there if you can't
     find it.  And, if you're not already, start using tags!
 
  2. You'll adapt this code to do simple heap corruption checking by calling into
     your heap checker (and leak checker if that is working) in the interrupt
     handler.  We'll maintain a range of code addresses we check (default: the 
     entire program) and a range of code addresses we do not check (initially:
     the entire `ckalloc.c` file) --- if you get interrupted in the former,
     run your heap checker and see if you can detect errors.

  3. As you notice, interrupt checking lets you transparently check an
     arbitrary predicate with *guaranteed* regularity without any
     code modifications.  This hack can serve you well in real life.
     However, a downside is that we will rarely get lucky and interrupt
     exactly when the invariant breaks (we can also miss if it breaks
     briefly).  We'll do a conceptually simple hack: for each load or
     store we discover (the two operations that can violate invariants)
     we replace them with a branch instruction that jumps to custom
     checking routine for that specific instruction address.  Because the
     jump is the same size as the load or store, the program binary does
     not change size ("dilate") and so we do not have to patch program
     addresses (infeasible if you can't reliably classify code and data).
     Because the trampoline is customized to that specific instruction,
     we can hard-code any knowledge it needs, including the return address
     it needs to jump back to.

     This means that when we interrupt a load or store once, the check
     will be performed for all future executions.  I.e., it is sticky,
     or persistent.

  4. While step (3) gives us much more control over the program, there
     are large blind spots until we instrument everything --- we will
     miss large chunks of code that only run for a short while or do
     not repeat.  We will build a trivial "restore-and-replay" setup to
     your code that will allow you restart your program over and over,
     until you have covered all of its code.  You can then check it on
     a thorough, final run.

     Checkpoint-and-restore is a powerful technique that has many
     applications but is also (IMO) wildly underutilized because it
     is extremely challenging to build on today's complex systems.
     This is another case of: simple pi system = simple implementation.

----------------------------------------------------------------------
## Part 0: setup your code


#### Make sure ckalloc works and migrate

You should pull your code from `6-debug-alloc`.

First: make sure things still work: make sure all your
tests are enabled!

        % cd 6-debug-alloc/code
        % make check

Second: copy the code over to today's lab:

        % cd ../9-memcheck-stat
        % cp ../6-debug-alloc/code .
        # make sure tests still work!
        % make check


Then merge in the starter code and make sure there is no difference.

        % cd 9-memcheck-stat/code
        % cp -r ../starter-code/* .
        % make check


You'll have to:
   1. Modify the `CFLAGS` variable in your `Makefile` to tell gcc not
      to reorder functions:

        CFLAGS += -fno-toplevel-reorder 

   2.  Also, add in the file: `ck-memcheck.c`


#### Make sure `timer-int` works and change to use vector base

The code is in `timer-int`: it's just cs140e's code copied over.

----------------------------------------------------------------------
## Part 1: interrupt based checking.

In this part, integrate the timer code with your checking code.
  1. You should check if the interrupted PC was within your memory 
     checking code.  You can figure out the code range by putting
     a sentinel routine at the start of `ckalloc.c`:

            void ckalloc_start(void) {}

     and a sentinel routine at the end:

            void ckalloc_end(void) {}

     And considering all code between them as not-checkable. 
     Note that we use a special `gcc` option so it does not
     reorder the routines!

  2. If the interrupt pc is within these bounds, skip checking and increment a `skipped` counter.
  3. If not, run the checking and increment a `checking` counter.

Implement the routines in `ck-memcheck.c` (described at the end of `ckalloc.h`).
Key routines:
  1. `ck_mem_init`: do any initialization.
  2. `ck_mem_on`: start checking.
  3. `ck_mem_off`: turn off checking.

Clients should be able to turn on checking and turn it off repeatedly.
They should also `panic` if `stat_check_init` was not called.

You'll have to copy in and modify the timer code.  Play around with the test to see
how quickly you can catch the error.  Also perhaps add other tests and other errors.

Notes:
    1. You probably should make a way to easily shrink down the clock period in the timer
       interrupt.
    2. As you make interrupts more frequent, your code can appear to "lock up" b/c it
       is either running too slowly or b/c you are doing something you should not in 
       the interrupt handler.
    3. If your interrupt handler uses data that is shared with non-interrupt code
       make sure it is `volatile` or you just do `put32` and `get32`.


Extension:
  1. Make it so you can transparently run all your old tests and have them pass.  A nice 
     thing about our checker is that it should not alter the behavior of existing programs
     other than making them slower.  So it's easy to do lots and lots of regressions.
     (Perhaps the easiest way to do this is modifying `cstart`; even slicker is to modify
      the `.bin`.)

----------------------------------------------------------------------
## Part 2: binary rewriting

The tradeoff with interrupt checking:
   1. The more frequently we run interrupts, the more accurate our checking, but the
      larger the overhead.   If you check often enough, this overhead can prevent
      your program from making any visible progress.
   2. The less frequently we run, the faster we go, the but more likely it is for us
      to miss errors.

Our partial hack for this will be to make our checks "sticky" in an
instruction once, we will rewrite it to call our checking code directly.
This way we do many more thorough checks than the interrupt handler can
hope to.



When we interrupt address `x`: 
  1. Check that we haven't already rewritten `x`.  
  2. If so, skip.
  3. Otherwise mark it (so we don't rewrite again) and generate a checking 
     trampoline (below).

The easiest way to understand the trampoline is to keep keep in mind the three
things it has to do:
  1. We need to call a checking routine.  
  2. Run the original instruction
  3. Jump back to 4 bytes past the rewritten instruction.

Thus, we need to generate the following using your
dynamic code generation knowledge from lab 2:
  1. Before we can call anything, save the caller-saved registers (
     since they are all live (just like interrupt handling).  Do this by 
     `push`ing  everything but `pc`.
  2. Call your monitoring routine using a `bl` instruction.  Make sure to move any 
     values you need to the right argument registers (`r0` through `r3`).
  3. Pop all registers.
  4. Insert the instruction at address `x` at this point so it gets executed.
  5. Insert a custom jump (`b` instruction`) back to address `x+4`.


We'll need one trampoline and one flag for each monitored instruction.
The easiest approach is to just preallocate these up front in an array
and then just index using the `pc` (make sure to subtract the beginning
of the range and divide by 4!).

The big issue --- we can't do this simple approach for all instructions
(e.g., it will break relative branches).  For speed we will only want
to do for loads and stores in any case since these are the operations
that actually cause an invariant to break.    Skipping ALU operations
is a significant speedup.

Because mistakes in dynamically generated code can be insanely hard to track down,
for today's lab we will take a ludicrously brain-dead, stupid approach:
  1. Look in your `part3-test.list`
  2. Pull out instruction encoding for the load and store instructions in 
     `test_check`.   
  3. Write your binary rewriter --- for today only! --- to 
     look for *only* those values and rewrite them.
  4. After you do a rewrite, dump out the values of the instructions in your
     trampoline and check with everyone else.

This approach will make it easy for use to cross-check and make sure we all have
the same method for encoding branches and branch-and-links.

Folow up:
  1. The obvious huge extension is to rewrite the instruction decoding so you can
     check arbitrary loads and stores.  The main tricky thing is making sure
     you don't rewrite those that involve the `pc` register (or so so carefully).
  2. A good way to thoroughly check your code is to use it to rewrite (but not check)
     all your previous pi programs and make sure their output does not change.  This
     is a ruthless check --- after you pass it, you'd be surprised if your rewriting code
     was broken.

----------------------------------------------------------------------
## Part 3: using `gcc`  to check every load and store: "address sanitizer"

This part of the lab bought to you by *Zach Yedidia*!

If you pass `-fsanitize=kernel-address` GCC will instrument loads and
stores with calls to the following functions depending on the type
of access:

* `void __asan_load1_noabort(unsigned long addr)`
* `void __asan_load2_noabort(unsigned long addr)`
* `void __asan_load4_noabort(unsigned long addr)`
* `void __asan_load8_noabort(unsigned long addr)`
* `void __asan_loadN_noabort(unsigned long addr, size_t sz)`

* `void __asan_store1_noabort(unsigned long addr)`
* `void __asan_store2_noabort(unsigned long addr)`
* `void __asan_store4_noabort(unsigned long addr)`
* `void __asan_store8_noabort(unsigned long addr)`
* `void __asan_storeN_noabort(unsigned long addr, size_t sz)`

I recommend defining a single function `asan_access(unsigned long addr, size_t sz, bool write)` and calling that from each `__asan*` function.

You also have to define the following functions as stubs (I'm not sure what they are for):

* `void __asan_handle_no_return()`
* `void __asan_before_dynamic_init(const char* module_name)`
* `void __asan_after_dynamic_init()`

I've defined them as empty functions.

# Tests

Some simple tests:

```c
static volatile int g;

void use_after_free() {
    printk("should panic in heap\n");

    int* p = (int*) ckalloc(sizeof(int));
    *p = 42;
    ckfree(p);
    g = *p; // should panic
}

void illegal_access() {
    printk("should panic in heap\n");

    int* p = (int*) ckalloc(sizeof(int) * 4);
    memset(p, 0, 4);
    g = p[4]; // should panic
    ckfree(p);
}

void legal_access() {
    printk("should not panic\n");

    int* p = (int*) ckalloc(sizeof(int) * 4);
    memset(p, 0, 4);
    g = p[3]; // should not panic
    ckfree(p);
}

void invalid_code() {
    printk("should panic on code segment\n");

    memset(invalid_code, 0, 8); // should panic
}
```
