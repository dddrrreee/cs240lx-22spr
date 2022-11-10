// weak test of malloc/free: checks that subsequent allocations are
// <sizeof (void *)> bytes from each other.

#ifndef __CKALLOC_H__
#define __CKALLOC_H__

#include "src-loc.h"

typedef enum {  ALLOCED = 11, FREED } state_t;
enum { REDZONE_NBYTES = 128, REDZONE_VAL = 0xfe };

// pull the remainder into the second redzone.
typedef struct ck_hdr {
    struct ck_hdr *next;
    uint32_t nbytes_alloc;  // how much the user requested to allocate.
    uint32_t state;          // state of the block: { ALLOCED, FREED }

    // unique block id: this lets us compare different runs even if the 
    // allocation location changes.  starts at 0.
    uint32_t block_id;

    src_loc_t alloc_loc;    // location they called ckalloc() at.

    // used for gc: i didn't cksum these.
    uint32_t refs_start;    // number of pointers to the start of the block.
    uint32_t refs_middle;   // number of pointers to the middle of the block.

    uint16_t mark;          // 0 initialize.
    
    uint8_t rz1[REDZONE_NBYTES];
} hdr_t;

// requires that p points to the start of the allocated region.
static unsigned ck_blk_id(void *p) {
    hdr_t *h = p;
    return h[-1].block_id;
}

// returns pointer to the first header block.
hdr_t *ck_first_hdr(void);

// returns pointer to next hdr or 0 if none.
hdr_t *ck_next_hdr(hdr_t *p);

// get the number of bytes for hdr <h>
static inline unsigned ck_nbytes(hdr_t *h) {
    return h->nbytes_alloc;
}
// get the start of allocated data for <h>
// [confusing routine name, sorry]
void *ck_hdr_start(hdr_t *h);
// get one past the end of allocated data for <h>
// [confusing routine name, sorry]
void *ck_hdr_end(hdr_t *h);

// pointer to first redzone.
static inline uint8_t *ck_get_rz1(hdr_t *h) {
    return h->rz1;
}

// pointer to the second redzone
static inline uint8_t *ck_get_rz2(hdr_t *h) {
    return ck_hdr_end(h);
}

// is <ptr> in block <h>?
unsigned 
ck_ptr_in_block(hdr_t *h, void *ptr);

// is pointer <p> on the allocated list?
hdr_t *ck_ptr_is_alloced(void *ptr);

#define ckalloc(_n) (ckalloc)(_n, SRC_LOC_MK())
#define ckfree(_ptr) (ckfree)(_ptr, SRC_LOC_MK())
void *(ckalloc)(uint32_t nbytes, src_loc_t loc);
void (ckfree)(void *addr, src_loc_t loc);

// integrity check the allocated / freed blocks in the heap
//
// returns number of errors in the heap.   stops checking
// if heap is in an unrecoverable state.
//
// TODO (extensions): 
//  - probably should have returned an error log, so that the 
//    caller could report / fix / etc.
//  - give an option to fix errors so that you can keep going.
int ck_heap_errors(void);


// public function: call to flag leaks (part 1 of lab).
//  - <warn_no_start_ref_p>: if true, will warn about blocks that only have 
//    internal references.
//
//  - returns number of bytes leaked (where bytes = amount of bytes the
//    user explicitly allocated: does not include redzones, header, etc).
unsigned ck_find_leaks(int warn_no_start_ref_p);

// mark and sweep: works similarly to ck_find_leaks, resets
// state of unrefernced blocks.
// 
// Invariant:
//  - it should always be the case that after calling ck_gc(), 
//    immediately calling ck_find_leaks() should return 0 bytes 
//    found.
unsigned ck_gc(void);

// These two routines are just used for testing:

// Expects no leaks.
void check_no_leak(void);
// Expects leaks.
unsigned check_should_leak(void);

// info about the heap useful for checking.
struct heap_info {
    // original start of the heap.
    void *heap_start;
    // end of active heap (the next byte we would allocate)
    void *heap_end;

    // ckfree increments this on each free.
    unsigned nbytes_freed;
    // ckmalloc increments this on each free.
    unsigned nbytes_alloced;
};

struct heap_info heap_info(void);


// can use this to do debugging that you can turn off an on.
#define ck_debug(args...) \
 do { if(ck_verbose_p) debug("CK_DEBUG:" args); } while(0)

// just emits an error.
#define ck_error(_h, args...) do {      \
        trace("ERROR:");\
        printk(args);                    \
        hdr_print(_h);                  \
} while(0)

// emit error, then panic.
#define ck_panic(_h, args...) do {      \
        trace("ERROR:");\
        printk(args);                    \
        hdr_print(_h);                  \
        panic(args);                    \
} while(0)

// shouldn't be in the header.
static void inline hdr_print(hdr_t *h) {
    trace("\tlogical block id=%u, [addr=%p] nbytes=%d\n", 
            h->block_id, 
            ck_hdr_start(h),
            h->nbytes_alloc);
    src_loc_t *l = &h->alloc_loc;
    if(l->file)
        trace("\tBlock allocated at: %s:%s:%d\n", l->file, l->func, l->lineno);

#if 0
    // next lab
    l = &h->free_loc;
    if(h->state == FREED && l->file)
        trace("\tBlock freed at: %s:%s:%d\n", l->file, l->func, l->lineno);
#endif
}

extern unsigned ck_verbose_p;
static inline void ck_verbose_set(int v) {
    ck_verbose_p = v;
}


/**************************************************************************
 * lab 9
 */


// initialize interrupt checking.
void ck_mem_init(void);

// turn int-mem checking on.
void ck_mem_on(void);
// turn int-mem checking off.
void ck_mem_off(void);

// limit checking to the pc addresses in [start,end)
void ck_mem_set_range(void *start, void *end);

// returns 1 if <pc> is in a range that should be checked,
// 0 otherwise.
int (ck_mem_checked_pc)(uint32_t pc);

// hack so you don't have to keep casting everywhere.
#define ck_mem_checked_pc(_x) (ck_mem_checked_pc)((uint32_t)_x)

// dump out stats about checking.  if <clear_stats_p>=1, then
// reset them.
unsigned ck_mem_stats(int clear_stats_p);


// put at start of ckalloc.c file
void ckalloc_start(void);
// put at end of ckalloc.c file
void ckalloc_end(void);

/*************************************************************************
 * lab 11 purify lab
 */
// given an adddresss returns a block if it's somewhere
// within [header, redzone 1 redzone 2 etc].
hdr_t *ck_get_containing_blk(void *addr);

// computes the offset <addr> is from allocated mem in <h>: 
//  - negative implies before the block starts, 
//  - positive implies after block ends.  
//  - 0 means no overflow.
static inline int ck_illegal_offset(hdr_t *h, void *addr) {
    char *s = ck_hdr_start(h); // start of allocated memory
    char *e = ck_hdr_end(h); // end of allocated memory
    char *p = addr;

    // before the block: return negative
    if(p < s)
        return p - s;

    // past the end: return positive
    if(p >= e)
        return p - e + 1;

    // <addr> is not illegal.
    return 0;
}

#endif
