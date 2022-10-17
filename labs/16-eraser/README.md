## Eraser

Today we're going to build a simple version of eraser (see the `docs`
directory).  To keep it simple, we will use fake threading calls that
indicate:
  - What thread id is running.
  - When a context switch occurs.
  - When a lock and unlock occurs.
  - When you allocate and free memory.

We do fake threading to make it easier to develop your tool in just a
few hours.  Among other things, doing it fake rather than real makes
it easy to separate out issues where you get race condition in your
tool, makes deterministic debugging trivial, and also side-steps issues
arising from bugs that you might have in your context switching or thread
package library.

Don't fear:  Once the tool works on fake code you can then retarget to
a real threads package.  

The checker will be built on our "memtrace" system from the 240lx lab 11
(`../11-memcheck-trap/`).   If you recall, that lab let you register a
handler that would be called on each load or store.  Eraser will work by:

  1. Tracking the locks held by the current thread (its "lockset").
  2. On each load or store to an address, intersect this lockset 
     the lockset stored in shadow memory for the address.
  3. If the result is empty: error.  If not, store the possibly smaller
     lockset back into the address's shadow memory.

The Eraser paper discusses different methods to reduce false positives.
You will implement these today.  The code in `checker-eraser.c` and
`tests-eraser` have a simple working version and some trivial tests that
should pass.  I'll push some more aggressive ones.

The cool thing about this lab is that you can build a working eraser in
a few hours using tracing.  The alternative of using a tool like Valgrind
or writing your own would take weeks to years (in the latter case).

Checkoff:
  - pass the tests.  

--------------------------------------------------------------------------------------
#### Eraser review

Recall how the eraser code works:
  1. Initially each word is put in a virgin state.
  2. When the first thread uses the word, Eraser uses shadow memory to
     record that the word is in in the  "shared exclusive" state.
     (Hack to handle the case where a thread has allocated an object
     and is initializing it before releasing it to other threads).
  3. When another thread reads it, the word is put in a "shared state".
     (Hack to handle the case where one thread initializes an object,
     but the object is then treated as a read-only object that thus does
     not need locking).
  4. When another thread writes it, the word is put in a "shared
     modified" state.  (I.e., this is the standard notion of shared
     memory that needs locks to prevent data races as
      multiple threads read and write it).

In states 2-4, the lockset is refined (intersected) with the current set
of locks a thread holds.   Errors are only emitted if the lockset is empty
and the word is in the shared-modified state or is transitioning to it.

As usual we will build up in a simple way.  
  1. Just refine locks and give a message if the lockset goes empty.
  2. Then add the shared exclusive state to handle initialization.
  3. Then add the shared state to handle read-only objects.
  4. If you can, also write some more tests for us to use!

----------------------------------------------------------------------
#### Part 0: trivial eraser

The current eraser files implement a trivial eraser that tracks a 
single piece of shared state.  The tests make sure that it gives no
errors when locks are held and it does when they are not held.

To make things easy, these are back in lab 11 in the code directory.

  - `eraser.h` gives the public interface.  You should implement these so the
    tests pass.
  - `checker-eraser.c` has the implementation.
  - `tests-eraser/fake-thread.h` is a fake thread "implementation"
    that calls into your eraser tool using the routines defined in
    `eraser.h`.  The tests use this header; things should "just work".

In terms of the Eraser paper's state machine, there are two states:
  1. Virgin (as in the paper) which transitions to Shared-modified on
     any access to a 32-bit word (read or write).
  2. Shared-modified: gives an error if the lockset for a given 
     32-bit word is empty (whether or not another thread can access the memory).

For simplicity, assume:
  1. We track exactly one piece of memory.
  2. We have at most one lock.
  3. We have exactly two threads.

Because of these trivializations you can use a few statically declared
global variables, no memory allocation, no set operations, no tricky
data structures.  You will just give an error if they touch the one
location in the heap without a lock.  Again, this would be a pretty
useless checker for an end-user, we do things this way so we can easily
debug your shadow memory and helper routines.

As a good first step add shadow memory:
  1. As in Eraser, we track things at a word level (rather than byte).
  2. The `state_t` structure defined in `eraser-internal.h` is the size
     of a word (4-bytes).
  3. Thus, for your shadow memory, you will allocate a region the same
     size as your heap.
  4. Given given an address `addr`, remove the lower two bits (to make
     it word-aligned) and simply add the offset between the heap and
     the shadow to find the state associated with `addr`.
  5. Note: This process is identical to your `memcheck` checker.
     The only thing that should change are the variable names.

For `eraser.h`:
  - `eraser_mark_alloc`: mark an address range as allocated.  Note that
    this range will always be at least 4-byte aligned and the `nbytes`
    always a multiple of 4-bytes.

    Note: by the time this gets called, `kmalloc` will have already
    `memset` the region to 0.  You may either want to initialize the
    memory region at the beginning to a new state (e.g., `IGNORE`) or
    just accept the somewhat confusing re-VIRGIN state transition that
    will happen.  I did the latter; but the former is cleaner.

  - `eraser_mark_free` will mark the region as `INVALID`.  We don't
     actually need this for the current test because of how our `kmalloc`
     works.  In a full-functioned allocator, the `free()` implementation
     would likely be modifying the freed memory (to put it on a freelist
     etc) --- if Eraser does not know the block is freed it will give
     a bunch of false error reports.

     One way to look at this is that once a pointer is passed to `free()`
     that block is now private (sort of like the Exclusive state in
     eraser) and so doesn't need locks to control access.

  - `eraser_lock` and `eraser_unlock` will be called by the thread package's
    locking routines.  This should add and remove locks from the lockset.

  - `eraser_mark_lock`: I was planning on using this for hea-allocated
    locks  to tell the tool to *not* track a lockset on a lock (since
    that makes not sense --- locks by design are read and written
    without holding a lock!).  Currently we just use global locks,
    so this doesn't matter.  This would have to be called by any lock
    initialization the thread's package does.  It may be better to just
    write `eraser_lock` and `eraser_unlock` in such a way that you don't
    have to do use this routine, however.

  - `eraser_set_thread_id` would be called by the thread's package to tell
    your tool it switched threads.

The tests are in `tests/part2-tests*.c`:
  - `0-eraser-test0.c` (no error) --- basic non-eraser test that makes sure
    you can still run memory tracing.
  - `0-eraser-test1.c` (no error) --- basic eraser test that makes sure
    you can still read and write with a lock held and get no error.
  - `0-eraser-test2-bug.c` (error) --- make sure that writing with no lock
    gives an error.
  - `0-eraser-test3-bug.c` (error) --- similar, with more locking.
  - `0-eraser-test4.c` (no error) --- no bug with a bunch of thread
    switching.

-----------------------------------------------------------------------------
#### Part 3: Shared-exclusive Eraser

The previous Eraser is pretty useless.  Let us at least track if a second
thread is touching memory before giving an error!     We'll use the
shared exclusive state as described in the paper for this.

When a thread T1 touches word `w` in a the `SH_VIRGIN` state:
   1. Transition to `SH_EXCLUSIVE`.  (I misspelled as `SH_EXLUSIVE` initially --- oy.)
   2. Store the current thread id in the associated state.
   3. For subsequent accesses by the same thread T1, stay in `SH_EXCLUSIVE`.
   4. For subsequent accesses by a different thread T2, transition to the 
      `SH_SHARED_MOD` state and initialize the variable's lockset to T2's lockset.
   5. If the lockset in `SH_SHARED_MOD` becomes empty (even on the initial 
      transition), give an error.

The tests will be added to `tests-eraser/3-eraser-tests*.c` --- the
first two tests are the same as above; if I was more clever we'd have
a way to flip their behaivor:

  - `3-eraser-test2-nobug.c` (no error) --- identical test as `part2-test3.c` but here we
    will *not* have an error because a second thread does not touch it.
  - `part2-test4.c` (no error) --- identical test as `part2-test4.c` but here
    we will not have an error because a second thread does not touch it.

  - `part2-test5.c` (error) --- similar, but the unlocked read is done
    by a second thread so you should flag an error.
  - `part2-test6.c` (no error) --- similar, but has the memory allocation 
    outside of the lock and only has one thread, so no error.

  - There are a bunch of other tests now, too --- just look at them :)

Note that the state structure only has a small 16-bit wod to track the
lockset (or in our case the single held lock).   I did this so that the
state could be 32-bits and the shadow memory calculations would be easy.
This does create a problem of how to map from a lock address to a small
integer.  I used the following dumb way to get from lock pointers to a
small integer that can be stored in a state:

    // should the empty lockset have an id?
    static uint16_t lock_to_int(void *lock) {
    #   define MAXLOCKS 32
        static void *locks[MAXLOCKS];

        // lock=<NULL> is always 0.
        if(!lock)
            return 0;
        for(unsigned i = 0; i < MAXLOCKS; i++) {
            if(locks[i] == lock)
                return i+1;
            if(!locks[i]) {
                locks[i] = lock;
                return i+1;
            }
        }
        panic("too many locks!!\n");
    }

As always, you are adults, so are welcome to do something less basic.

--------------------------------------------------------------------------------------
#### Part 4: Shared Eraser

Now add the shared state.  Recall this state was used to handle the common
case where one thread intialized data, and then subsequent accesses by
all other threads were read-only and thus did not use locks.  Tests are in
`part4-tests*.c`.  The basic idea: 
  1. On write in Exclusive you'll transition to Shared Modified (as above).
  2. On read in Exclusive you'll now transition to Shared.  In shared you keep
     refining the lockset, but do not give an error if it becomes empty.  If 
     a write happens, you transition to Shared Modified and (as always) immediately
     give an error for an empty lockset.

--------------------------------------------------------------------------------------
#### Find other race bugs.

Useful checks:
  - Track if locks are double acquired, releases without acquisition, held "too long",
     acquired and released with no shared state touched (this often will point out bugs 
     in the tool).
  - Deadlock detection by adding edges to a hash table and detecting cycles.

Both of these find real bugs in real code.  Recommended.

--------------------------------------------------------------------------------------
#### Other Extensions:  Tons.

There are obviously all sorts of extensions.  
  0. Handle multiple locks!  Multiple threads!  Multiple allocations!

  1. Handle fork/join code that has "happens-before" constraints (e.g.,
     code cannot run before it is forked, code that does a join cannot
     run before the thread it waits on has exited).  Adding these features
     can remove a bunch of false positives.

  2. The shared-excluse state is a bad hack  --- a thread can run for
     milliseconds (millions of instructions) before a thread switch.
     This potentially opens up all sorts of false negatives.  One easy
     hack to counter: use your single-stepping code to force a context
     switch after a small number of instructions (or, perhaps, as soon as
     the initializing thread returns from the initialization routine).
     A second is to use your garbage collection pointer scanner to
     determine which addresses can be seen by another thread (e.g.,
     as soon as they are reachable from a heap data structure).

  3. Add checking to detect when interrupt handlers read/write memory
     that threads are modifying as well.  I don't believe the eraser
     description works (but perhaps I am incorrec!) so you may have to
     think of an alternative approach.

  4. Lots of other ones to make this tool more useful.  It's obviously pretty crude.
