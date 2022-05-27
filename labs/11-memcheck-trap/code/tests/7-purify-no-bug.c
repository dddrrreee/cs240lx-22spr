// no bug: allocate a couple blocks, memset, free.
#include "rpi.h"
#include "purify.h"

unsigned expected = 45;

static int fn(void) {
    // zero initialized by kmalloc.
    volatile int *x = purify_alloc(4);
    for(int i = 0; i < 10; i++)
        *x += i;
    printk("*x = %d\n", *x);
    assert(get32(x) == expected);
    return *x;
}

void notmain(void) {
    trace("should have no error: running w verbose off\n");
    purify_init();
    purify_verbose_set(0);

    if(fn() != expected)
        panic("invalid result\n");

    trace("SUCCESS: no bug\n");
}
