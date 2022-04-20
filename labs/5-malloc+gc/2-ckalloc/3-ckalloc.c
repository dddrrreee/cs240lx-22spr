// weak test of malloc/free: checks that subsequent allocations are
// <sizeof (void *)> bytes from each other.
#include "test-interface.h"
#include "ckalloc.h"

void notmain(void) {
    void *p = ckalloc(4);
    ckfree(p);
}
