// weak test of malloc/free: checks that subsequent allocations are
// <sizeof (void *)> bytes from each other.
#include "test-interface.h"

union align {
        double d;
        void *p;
        void (*fp)(void);
};


typedef union header { /* block header */
    struct {
            union header *ptr; /* next block if on free list */
            unsigned size; /* size of this block */
    } s;
    union align x; /* force alignment of blocks */
} Header;


void notmain(void) {
    // output("doesnt do anything yet\n");
    // return;

    char *p0 = kr_malloc(1);
    unsigned n = sizeof (Header) + sizeof(union align);

    // panic("n=%d\n", n);
    int ntests = 10;

    output("malloc(1) = %p\n", p0);
    for(int i = 0; i < ntests; i++) { 
        char *p1 = kr_malloc(1);

        output("malloc(1) = %p\n", p1);

        assert(p0>p1);
        if(p0 - n != p1)
            output("p0=%p, p1=%p, expected diff=%d, actual=%d\n", p0,p1,n, p0-p1);
        else
            output("passed iter %d\n", i);
		p0 = p1;
    }
    trace("success: malloc/free appear to work on %d tests!\n", ntests);
}
