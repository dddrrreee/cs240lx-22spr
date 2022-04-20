// use after free.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test4: use after free\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);


    void *p0 = ckalloc(rpi_rand32() % 1024);
    void *p1 = ckalloc(rpi_rand32() % 1024);

    ckfree(p0);
    ckfree(p1);

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");
}
