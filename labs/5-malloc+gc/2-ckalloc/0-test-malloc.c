// weak test of malloc/free: checks that subsequent allocations are
// <sizeof (void *)> bytes from each other.
#include "test-interface.h"

void notmain(void) {
    enum { ntests = 10 };
    
    char *p0 = ckalloc(1);
    for(unsigned i = 0; i < ntests; i++)  {
        char *p1 = ckalloc(1);

        output("malloc(1)=%p, diff=%ld\n", kr_malloc(1), p1-p0);

        // see that we get back the previous.
        ckfree(p1);
        char *p2 = ckalloc(1);
        if(p1 != p2)
            panic("expected to reallocate freed ptr %p\n", p1);
        p0=p1;
        trace("iter=%d: trivial free/alloc worked as expected\n", i);
    }

    trace("success: malloc/free appear to work on %d tests!\n", ntests);
}
