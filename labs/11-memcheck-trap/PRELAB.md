### Prelab for trap based memchecking.



We've typically had you to copy old files into each new lab.  This
approach is good in that it doesn't mess with working code, but it's
obviously bad in that we have duplicate stuff everywhere.  What we should
be doing in general is:
  1. Develop pieces in isolation.
  2. Test until we're happy.
  3. Commit the code to `libpi`.

In any case, we'll do this housekeeping now. 

Move `7-mem-protection/code`pinned-vm.c` into `libpi/src`:

  1. First make sure the tests in `7-mem-protection/code' work.
  2. Move your `pinned-vm.c` into your `libpi/src`.
  3. Change the `code/Makefile` to no longer use the local `pinned-vm.c`
  4. Make sure your tests pass.

Move `6-debug-alloc/ckalloc.c`  into `libpi/src`:
  1. First make sure the tests in `6-debug-alloc/code` pass.
  2. Move your `ckalloc.c` into your `libpi/src`.
  3. Change the `code/Makefile` to no longer use the local `pinned-vm.c`
  4. Make sure your tests pass.

Good!  Now things will be cleaner for fixing.
