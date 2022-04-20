// trivial program to start checking mark/sweep with.
//  1. allocate one block with a global pointer
//  2. allocate one block with a stack (or reg) pointer.
//  3. make sure no leaks detected.
#include "rpi.h"
#include "ckalloc.h"

#if 0
static void check_no_leak(void) {
    if(ck_heap_errors())
        panic("GC: invalid error!!\n");
    else
        trace("GC: SUCCESS heap checked out\n");
    if(ck_find_leaks(1))
        panic("GC: should have no leaks!\n");
    else
        trace("GC: SUCCESS: no leaks!\n");
}
#endif

char *global_p;
void * test(void) {
	extern uint32_t __data_start__, __data_end__;

    printk("global_p=%p datastart=%p, dataend=%p\n",&global_p, &__data_start__, &__data_end__); 
    global_p = ckalloc(4);
    memset(global_p, 0, 4);
    
    void *p= ckalloc(4);

    check_no_leak();

    // have to return p or the compiler gets rid of it.
    return p;
}


void notmain(void) {
    printk("GC test2: checking that local (stack or reg) pointers are detected.\n");


    // allocated on the stack. <--- might not be using it.
    printk("last alloc =%p\n", test());
}

