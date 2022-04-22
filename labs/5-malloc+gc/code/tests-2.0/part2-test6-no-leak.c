// allocate, move pointer to the center, make usre it doesn't get lost.
//
// we should probably split into different files so the compiler does 
// not kill things off and remove the maybe leak?
//
// written by jimpo, 5/19/20
#include "rpi.h"
#include "ckalloc.h"

char *test1(void) {
    // allocate then move pointer to middle of allocated region.
    char *p = ckalloc(4);
    trace("alloc returned block id=%u [%p]\n", ck_blk_id(p), p);
    return p + 1;
}

void notmain(void) {
    printk("GC test6\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    char *p = test1();
    // print something so compiler is forced to save p.
    trace("test1 returned [%p]\n", p);

    // this will not warm if you point to the center.
    if(ck_find_leaks(0))
        panic("should have no definite leaks!\n");

    if(ck_find_leaks(1))
        trace("SUCCESS: detected a maybe leak!\n");
    else
        panic("should have found a maybe leak of: %p!\n", p);
}

