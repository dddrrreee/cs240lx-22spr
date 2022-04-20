// using a doubly linked list to make sure we can handle cycles.
#include "rpi.h"
#include "ckalloc.h"

#define N 100

// allocate a linked list.  do a leak detection.
void *test(void) {
    struct list {
        int x; 
        struct list *next;
        struct list *prev;
    } *h = 0;

    for(int i = 0; i < 100; i++) {
        struct list *e = ckalloc(sizeof *h);
        memset(e, 0, sizeof *e);
        e->x = 0;
        if(h) {
            assert(!h->prev);
            h->prev = e;
            e->next = h;
        }
        h = e;
    }
    // compiler optimizes this stuff away.
    // h= 0;
    // even this:
    //  memset(&h, 0, sizeof h);
    // perhaps if we put it in another file?
    check_no_leak();
    // return to try to get around compiler tricks.
    return h;
}

void notmain(void) {
    printk("GC test: checking that a doubly-linked list is handled.\n");
    // note: if we do a check after this, may miss b/c the value is laying around
    // on the stack or reg
    printk("last alloc =%p\n", test());
}
