
What to do:
  1. You have to copy over your ckalloc and kr-malloc.
  2. You'll have to  change `ck_ptr_is_allocated` to return the header it
     found the pointer in.

