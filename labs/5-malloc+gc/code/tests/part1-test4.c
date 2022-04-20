// use after free.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test4: use after free\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);

    char *p = ckalloc(4);
    memset(p, 0, 4);
    ckfree(p); 

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");

    p[4] = 1;   // past end of block

    if(ck_heap_errors() != 1)
        panic("invalid error!!\n");
    else
        trace("SUCCESS: detected corruption\n");
}
