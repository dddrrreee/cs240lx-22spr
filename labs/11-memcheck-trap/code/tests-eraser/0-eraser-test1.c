// engler: simple lock and unlock with one piece of shared state: should have no errors.
#include "eraser.h"

// has the lock/unlock etc implementation.
#include "fake-thread.h"

static int l;

void notmain() {
    eraser_init();
    eraser_set_thread_id(1);

    lock(&l);
    int *x = kmalloc(4);
    put32(x,0x12345678);   // should be fine.

    int ret = get32(x);    // should be fine.
    unlock(&l);

    trace_clean_exit("success!!\n");
}
