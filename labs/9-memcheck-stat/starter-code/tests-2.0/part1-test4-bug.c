// use after free.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test4: use after free\n");
    char *p = ckalloc(4);
    memset(p, 0, 4);
    ckfree(p); 

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");

    p[3] = 1;   // inside the block, but after free.

    if(ck_heap_errors() != 1)
        panic("invalid error!!\n");
    else
        trace("SUCCESS: detected corruption on block %u [%p]\n", ck_blk_id(p), p);
}
