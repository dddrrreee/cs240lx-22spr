// engler: should get an error b/c the new thread has no lock.
#include "eraser.h"

// has the lock/unlock etc implementation.
#include "fake-thread.h"

static int l,l2;

void notmain() {

    eraser_init();
    eraser_set_thread_id(1);
    int *x = kmalloc(4);

    lock(&l);
    put32(x,0x12345678);   // should be fine.

    unlock(&l);

    eraser_set_thread_id(2);
    trace("should *not* have an error because no other thread touches\n");
    put32(x,0x12345678);   // bug b/c no lock held.

    trace("SUCCESS: load should pass\n");
}
