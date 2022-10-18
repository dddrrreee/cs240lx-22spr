// engler: should have no error and be in SHARED-MOD state.
#include "eraser.h"

// has the lock/unlock etc implementation.
#include "fake-thread.h"

static int l1,l2;

void notmain() {

    eraser_init();
    eraser_set_thread_id(1);
    int *x = kmalloc(4);

    lock(&l1);
    put32(x,0x12345678);   // should be fine.
    unlock(&l1);

    eraser_set_thread_id(2);
    trace("should *not* have an error because no other thread touches\n");

    // goes to shared mod, but has a consistent lock so is ok.
    lock(&l2);
    put32(x,0x12345678);   // should be fine.
    put32(x,0x12345678);   // should be fine.
    put32(x,0x12345678);   // should be fine.
    put32(x,0x12345678);   // should be fine.
    put32(x,0x12345678);   // should be fine.
    put32(x,0x12345678);   // should be fine.
    unlock(&l2);

    trace("SUCCESS: load should pass\n");
}
