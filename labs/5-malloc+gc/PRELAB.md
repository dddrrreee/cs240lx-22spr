### Background on memory allocation and garbage collection


Some quick-read lectures:

   - [CMU lecture](docs/lecture-mem-alloc.pdf) on both allocation and Boehm GC.
   - [Stanford cs140](docs/cs140.malloc-free.pdf) main on allocation and 
     fragmentation.

A more thorough writeup by Boehm on today's hack:
   - [Boehm GC](docs/boehm.pdf).

Some common special cases:
   1. "object-caching" where you have per-size freelists that you can quickly add-to, take-from.
      This is used in linux (and other OSes) alot.
   2. "arenas" which you use when you are going to free a bunch of allocations
      at the same time.  

Some details:
   - [Arena](docs/arena.pdf) --- perhaps the first paper on arena allocation?  Fast read.
   - [Slab](docs)/191.pdf) --- basis of linux "slab" allocator.
   - [actual slab](docs/bonswick_slab.pdf) --- initial paper on linux "slab" allocator.
   - [high level slab writeup](https://www.kernel.org/doc/gorman/html/understand/understand011.html)

Some code:

  - [Google's tcmalloc](https://github.com/google/tcmalloc), 
    which alot of people seem to like:
  - [slab](https://github.com/torvalds/linux/blob/master/mm/slab.c), the actual source
    for linux slab.  A fair amount of code, but more understandable than a lot of linux
    at this point.
