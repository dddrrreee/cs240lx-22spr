// trivial program to check that you catch memory corruptions.
//
// in real life we would have all these different initialization 
// routines exposed etc, but we do this so it's clear what is going
// one / easy to change exactly what is occuring.
#include "rpi.h"
#include "ckalloc.h"

#define N 80

// should have no checks
void test_nullcheck(void) { 
    ck_mem_on();


    volatile unsigned *p = ckalloc(4);
    for(int i = 0; i < 10000; i++)
        *p;

    ck_mem_off();

    ck_mem_stats(1);
}
void test_nullcheck_end(void) {  }

void notmain(void) {
    printk("test1\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);
    ck_mem_init();

    // should have no checks.
    ck_mem_set_range(test_nullcheck, test_nullcheck_end);
    test_nullcheck();

#if 0
    ck_mem_set_range(test_check, test_check_end);
    test_check();
#endif

    printk("about to call again\n");
    if(ck_heap_errors())
        panic("FAIL: no heap errors??\n");
    else
        trace("SUCCESS heap checked out\n");
}
