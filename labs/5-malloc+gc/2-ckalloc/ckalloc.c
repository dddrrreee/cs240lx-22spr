// implement a simple ckalloc/free that adds ckhdr_t to the 
// allocation.

#include "test-interface.h"
#include "ckalloc.h"

// keep a list of allocated blocks.
static hdr_t *alloc_list;

// returns pointer to the first header block.
hdr_t *ck_first_hdr(void) {
    return alloc_list;
}

// returns pointer to next hdr or 0 if none.
hdr_t *ck_next_hdr(hdr_t *p) {
    if(p)
        return p->next;
    return 0;
}


void *ck_hdr_start(hdr_t *h) {
    return &h[1];
}

// one past the last byte of allocated memory.
void *ck_hdr_end(hdr_t *h) {
    return (char *)ck_hdr_start(h) + ck_nbytes(h);
}

// is ptr in <h>?
unsigned ck_ptr_in_block(hdr_t *h, void *ptr) {
    unimplemented();
}


int ck_ptr_is_alloced(void *ptr) {
    for(hdr_t *h = ck_first_hdr(); h; h = ck_next_hdr(h))
        if(ck_ptr_in_block(h,ptr)) {
            output("found %p in %p\n", ptr, h);
            return 1;
        }
    return 0;
}


// free a block allocated with <ckalloc>
void (ckfree)(void *addr, src_loc_t l) {
    hdr_t *h = (void *)addr;
    h -= 1;

    if(h->state != ALLOCED)
        loc_panic(l, "freeing unallocated memory: state=%d\n", h->state);
    loc_debug(l, "freeing %p\n", addr);
    
    h->state = FREED;
    unimplemented();
    assert(ck_ptr_is_alloced(addr));
    kr_free(h);
}

// interpose on kr_malloc allocations and
//  1. allocate enough space for a header and fill it in.
//  2. add the allocated block to  the allocated list.
void *(ckalloc)(unsigned nbytes, src_loc_t l) {

    hdr_t *h = kr_malloc(nbytes + sizeof *h);
    h->nbytes_alloc = nbytes;
    h->state = ALLOCED;
    h->alloc_loc = l;

    memset(h, 0, sizeof *h);
    loc_debug(l, "allocating %p\n", h);

    unimplemented();
}
