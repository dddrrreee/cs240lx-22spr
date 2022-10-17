// engler: do several thread switches: should have no error.
#include "eraser.h"

// has the lock/unlock etc implementation.
#include "fake-thread.h"

static int l;

void notmain() {

    eraser_init();
    int *x = kmalloc(4);

    eraser_set_thread_id(1);
    lock(&l);
    put32(x,0x12345678);   // should be fine.
    unlock(&l);

    eraser_set_thread_id(2);
    lock(&l);
    put32(x,2);   // should be fine.
    unlock(&l);

    eraser_set_thread_id(3);
    lock(&l);
    put32(x,3);   // should be fine.
    unlock(&l);

    trace("SUCCESS: stores should pass\n");
}
