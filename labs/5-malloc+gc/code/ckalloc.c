// implement a simple ckalloc/free that adds ckhdr_t to the 
// allocation.

#include "test-interface.h"
#include "ckalloc.h"




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
    return NULL;
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


hdr_t* ck_ptr_to_hdr(void* ptr) {
	for(hdr_t *h = ck_first_hdr(); h; h = ck_next_hdr(h)) {
        if(ck_ptr_in_block(h,ptr)) {
            trace("found %p in %p\n", ptr, h);
            return h;
        }
	}
    return NULL;
}

hdr_t* ck_ptr_is_alloced(void *ptr) {
    for(hdr_t *h = ck_first_hdr(); h; h = ck_next_hdr(h)) {
        if(ck_ptr_in_block(h,ptr) && h->state == ALLOCED) {
            trace("found allocated %p in %p\n", ptr, h);
            return h;
        }
	}
    return NULL;
}

//returns 1 if redzones are invallid, 0 otherwise
int ck_check_redzones(hdr_t *h, const void* sentinal) {
	uint8_t *rz2 = ck_get_rz2(h);
	int rz1_res = memcmp((void*)h->rz1, sentinal, REDZONE_NBYTES);
	int rz2_res = memcmp((void*)rz2, sentinal, REDZONE_NBYTES);


	return (rz1_res || rz2_res) ? 1 : 0;
}

//returns 1 if user writen data segment does not match redzone sentinal on a freed block
int ck_check_udata(hdr_t *h) {
	
	// if not freed block, check  makes no  sense
	if (h->state != FREED)
		return 0;

	uint8_t *data = (uint8_t*)(char*)h + sizeof(hdr_t);
	for (unsigned i = 0; i < h->nbytes_alloc; i++) {
		if (data[i] != REDZONE_VAL) {
			return 1;
		}
	}
	return 0;
}

int ck_heap_errors(void) {
	trace("checking heap for errors...\n");
	hdr_t *curr = ck_first_hdr();
	uint8_t sentinal_buf[REDZONE_NBYTES];
	const void* SENTINAL = memcpy(&sentinal_buf, curr->rz1, REDZONE_NBYTES);
	unsigned errs = 0;
	while (curr) {
		if (ck_check_redzones(curr, SENTINAL)) {
			trace("redzones incrmenting erros");
			// invalid redzones found
			errs++;
		}  
		if (ck_check_udata(curr)) {
			trace("udata incrementing errs.\n");
			errs++;
		}
		curr = ck_next_hdr(curr);
	}

	return errs;
}


// free a block allocated with <ckalloc>
void (ckfree)(void *addr, src_loc_t l) {
    hdr_t *h = addr - sizeof(hdr_t);
    if(h->state != ALLOCED)
        loc_panic(l, "freeing unallocated memory: [addr = %p state=%d]\n", addr, h->state);
    loc_debug(l, "freeing block = %d [addr = %p]\n", h->block_id, addr);
    assert(ck_ptr_is_alloced(addr) != 0);
    h->state = FREED;
	uint8_t sentinal_buf[REDZONE_NBYTES];
	for (unsigned i = 0; i < REDZONE_NBYTES; i++)
		sentinal_buf[i] = REDZONE_VAL;

	//check for redzone corruption
	if (ck_check_redzones(h, &sentinal_buf))
		panic("Error: redzone corruption detected on free of block = %d [addr = %x].\n", h->block_id, h);
	memset(addr, REDZONE_VAL, h->nbytes_alloc); //set free data to sentinal val
	
    assert(!ck_ptr_is_alloced(addr));
    // kr_free(h);
}

// interpose on kr_malloc allocations and
//  1. allocate enough space for a header and fill it in.
//  2. add the allocated block to  the allocated list.
void *(ckalloc)(uint32_t nbytes, src_loc_t l) {

    hdr_t *h = kr_malloc(nbytes + sizeof(hdr_t) + REDZONE_NBYTES); //first redzone is in header (hdr_t)
	memset(h, 0, sizeof *h);
	memset(h->rz1, REDZONE_VAL, REDZONE_NBYTES); // set  redzone1 to sentinal val


    h->nbytes_alloc = nbytes;
    h->state = ALLOCED;	
    h->alloc_loc = l;
	h->next = NULL;
	h->mark  = 0;

	memset((void*) ck_get_rz2(h), REDZONE_VAL, REDZONE_NBYTES);  //set redzone2 to sentinal  val

	// set block id
	h->block_id = next_block_id;
	next_block_id++;
    
    loc_debug(l, "allocating block = %d [addr = %p]\n", h->block_id, (void*)h + sizeof(hdr_t));

	list_append(alloc_list, h);

	return ((void*)h + sizeof(hdr_t));
	
}
