// trivial program to check that you catch memory corruptions.
//
// in real life we would have all these different initialization 
// routines exposed etc, but we do this so it's clear what is going
// one / easy to change exactly what is occuring.
#include "rpi.h"
#include "ckalloc.h"

#define N 80

void test_check(void) { 
    volatile char *p = ckalloc(N);
    ckfree((void*)p);
    ck_mem_on();
    for(int j = 0; j < 1000; j++) {
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
        *p = 1;
    }
    ck_mem_off();
    if(!ck_mem_stats(1))
        panic("no checks happened??\n");
}
void test_check_end() { }

void notmain(void) {
    printk("test1\n");

    // start heap allocating after the first mb.   give it 1mb to use.
    kmalloc_init_set_start(0x100000);
    unsigned n = 1024*1024;
    ck_init(kmalloc(n),n);
    ck_mem_init();

    ck_mem_set_range(test_check, test_check_end);
    test_check();

    printk("about to call again\n");
    if(ck_heap_errors())
        panic("FAIL: no heap errors??\n");
    else
        trace("SUCCESS heap checked out\n");
}
