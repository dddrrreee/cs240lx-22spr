// written by jimpo, 5/19/20
#include "rpi.h"
#include "ckalloc.h"

char *test1(void) {
    // allocate then move pointer to middle of allocated region.
    char *p = ckalloc(4);
    trace("alloc returned %p\n", p);
    memset(p, 0, 4);
    return p + 1;
}

void notmain(void) {
    printk("GC test6\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    char *p = test1();
    // print something so compiler is forced to save p.
    trace("test1 returned %p\n", p);

    // this will not warm if you point to the center.
    if(ck_find_leaks(0)) {
        panic("should have no definite leaks!\n");
    }

    if(ck_find_leaks(1)) {
        trace("SUCCESS: detected a maybe leak!\n");
    } else {
        panic("should have found a maybe leak!\n");
    }
}
