// use after free.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test3: use after free\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);

    char *p = ckalloc(4);
    memset(p, 0, 4);
    ckfree(p);  // should catch this.

    p[0] = 1;   // past end of block
    unsigned nerr;
    if((nerr = ck_heap_errors()) != 1)
        panic("invalid error: have %d errors expected 1!!\n", nerr);
    else
        trace("SUCCESS: detected corruption\n");
}
