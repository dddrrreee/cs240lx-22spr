// trivial program to start: allocate a single block, free it, check the
// heap
//
// there should be no bugs.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test1\n");

    char *p = ckalloc(4);
    trace("alloc returned %u [%p]\n", ck_blk_id(p), p);
    memset(p, 0, 4);
    ckfree(p);

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");
}
