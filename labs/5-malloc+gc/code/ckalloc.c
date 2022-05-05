// implement a simple ckalloc/free that adds ckhdr_t to the 
// allocation.

#include "test-interface.h"
#include "ckalloc.h"

// keep a list of allocated blocks.
static hdr_t *alloc_list;
static hdr_t *last_node = NULL;

static void list_remove(hdr_t *l, hdr_t *h) {
    assert(l);
    hdr_t *prev = l;
 
    if(prev == h) {
        l = h->next;
        return;
    }

    hdr_t *p;
    while((p = ck_next_hdr(prev))) {
        if(p == h) {
            prev->next = p->next;
            return;
        }
        prev = p;
    }
    panic("did not find %p in list\n", h);
}

static void list_append(hdr_t *l, hdr_t *h) {
	if (last_node == NULL) {
		//no nodes yet
		alloc_list = h;
		last_node = h;
		return;
	} else {
		assert(l);
		hdr_t *prev_last = last_node;
		last_node = h;
		prev_last->next = h;
		return;
	}
}

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
	if (ptr <= (void *)h + h->nbytes_alloc + sizeof *h && ptr >= (void*)h + sizeof *h) {
		return 1;
	}
	return 0;
}


hdr_t* ck_ptr_is_alloced(void *ptr) {
    for(hdr_t *h = ck_first_hdr(); h; h = ck_next_hdr(h)) {
        if(ck_ptr_in_block(h,ptr) && h->state == ALLOCED) {
            // output("found %p in %p\n", ptr, h);
            return h;
        }
	}
    return NULL;
}


// free a block allocated with <ckalloc>
void (ckfree)(void *addr, src_loc_t l) {
    hdr_t *h = addr - sizeof(hdr_t);

    if(h->state != ALLOCED)
        loc_panic(l, "freeing unallocated memory: addr = %p state=%d\n", addr, h->state);
    loc_debug(l, "freeing %p\n", addr);
    assert(ck_ptr_is_alloced(addr) != 0);
    h->state = FREED;
	list_remove(alloc_list, h);
    assert(!ck_ptr_is_alloced(addr));
    kr_free(h);
}

// interpose on kr_malloc allocations and
//  1. allocate enough space for a header and fill it in.
//  2. add the allocated block to  the allocated list.
void *(ckalloc)(uint32_t nbytes, src_loc_t l) {

    hdr_t *h = kr_malloc(nbytes + sizeof(hdr_t));
	memset(h, 0, sizeof *h);
    h->nbytes_alloc = nbytes;
    h->state = ALLOCED;	
    h->alloc_loc = l;
	h->next = NULL;
	h->mark  = 0;

    
    loc_debug(l, "allocating %p\n", h);

	list_append(alloc_list, h);
	return ((void*)h + sizeof(hdr_t));
	
}
