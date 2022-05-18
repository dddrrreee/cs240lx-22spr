// use after free.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test4: larger alloc, no error\n");

    void *p0 = ckalloc(rpi_rand32() % 1024);
    void *p1 = ckalloc(rpi_rand32() % 1024);
    ckfree(p0);
    ckfree(p1);

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");
}
