// use after free.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test4: use after free\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);


    enum { N = 6 };
    void *p[N];
    unsigned sz[N];

    for(int i = 0; i < N; i++) {
        sz[i] = rpi_rand32() % 1024;
        p[i] = ckalloc(sz[i]);
    }

    for(int i = 0; i < N; i++)
        ckfree(p[i]);

    if(ck_heap_errors())
        panic("invalid error!!\n");
    else
        trace("SUCCESS heap checked out\n");

    int i = rpi_rand32() % N;
    char *x = p[i];
    unsigned off = sz[i] + rpi_rand32() % 32;
    x[off] = 1;
    trace("corrupting %p at offset %d (%p)\n", x, off, &x[off]);

    if(!ck_heap_errors())
        panic("missed error!\n");
    else
        trace("SUCCESS found error\n");
}
