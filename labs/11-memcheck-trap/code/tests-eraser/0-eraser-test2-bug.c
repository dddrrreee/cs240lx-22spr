// engler: do one safe store and one unsafe load.
#include "eraser.h"

// has the lock/unlock etc implementation.
#include "fake-thread.h"

static int l;

void notmain() {
    eraser_init();
    eraser_set_thread_id(1);

    int *x = kmalloc(4);

    trace("--------------------------------------------------\n");
    trace("expect a store error at pc=%p, addr=%p\n", put32, x);
    trace("--------------------------------------------------\n");
    put32(x,0x12345678);   // trivial error.

    panic("should have caught unprotected store error\n");

}
