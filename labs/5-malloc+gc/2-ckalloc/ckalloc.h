// weak test of malloc/free: checks that subsequent allocations are
// <sizeof (void *)> bytes from each other.

#ifndef __CKALLOC_H__
#define __CKALLOC_H__

#include "src-loc.h"

typedef enum {  ALLOCED = 11, FREED } state_t;

// pull the remainder into the second redzone.
typedef struct ck_hdr {
    struct ck_hdr *next;
    uint32_t nbytes_alloc;  // how much the user requested to allocate.
    uint32_t state;          // state of the block: { ALLOCED, FREED }

    src_loc_t alloc_loc;    // location they called ckalloc() at.

    // used for gc: i didn't cksum these.
    uint32_t refs_start;    // number of pointers to the start of the block.
    uint32_t refs_middle;   // number of pointers to the middle of the block.

    uint16_t mark;          // 0 initialize.
} hdr_t;

// returns pointer to the first header block.
hdr_t *ck_first_hdr(void);

// returns pointer to next hdr or 0 if none.
hdr_t *ck_next_hdr(hdr_t *p);

// get the number of bytes for hdr <h>
static inline unsigned ck_nbytes(hdr_t *h) {
    return h->nbytes_alloc;
}
// get the start of allocated data for <h>
void *ck_hdr_start(hdr_t *h);
// get the end of allocated data for <h>
void *ck_hdr_end(hdr_t *h);

// is <ptr> in block <h>?
unsigned 
ck_ptr_in_block(hdr_t *h, void *ptr);

// is pointer <p> on the allocated list?
int ck_ptr_is_alloced(void *ptr);

#define ckalloc(_n) (ckalloc)(_n, SRC_LOC_MK())
#define ckfree(_ptr) (ckfree)(_ptr, SRC_LOC_MK())
void *(ckalloc)(uint32_t nbytes, src_loc_t loc);
void (ckfree)(void *addr, src_loc_t loc);

#endif
