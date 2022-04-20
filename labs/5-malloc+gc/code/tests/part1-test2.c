// checks a single memory overflow.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test2: memory overflow at offset 4\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);

    char *p = ckalloc(4);
    memset(p, 0, 4);
    p[4] = 1;   // past end of block
    ckfree(p);  // should catch this.
}
