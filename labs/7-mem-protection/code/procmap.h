#ifndef __PROC_MAP_H__
#define __PROC_MAP_H__

// physical address of our kernel: used to map.
// probably should tag with the domain?
typedef struct {
    uint32_t addr, nbytes;
    enum { MEM_DEVICE, MEM_RW, MEM_RO } type;
    unsigned dom;
} pr_ent_t;
static inline pr_ent_t
pr_ent_mk(uint32_t addr, uint32_t nbytes, int type, unsigned dom) {
    demand(nbytes == 1024*1024, only doing sections first);
    return (pr_ent_t) {
        .addr = addr,
        .nbytes = nbytes,
        .type = type,
        .dom = dom
    };
}

typedef struct {
#   define MAX_ENT 8
    unsigned n;
    pr_ent_t map[MAX_ENT];
} procmap_t;
// add entry <e> to procmap <p>
static inline void procmap_push(procmap_t *p, pr_ent_t e) {
    assert(p->n < MAX_ENT);
    p->map[p->n++] = e;
}

// memory map of address space.  each chunk of memory should have an entry.
// must be aligned to the size of the memory region.
//
// we don't seem able to search in the TLB if MMU is off.  
// 
// limitations in overall scheme:
//   - currently just tag everything with the same domain.
//   - should actually track what sections are allocated.
//   - basic map of our address space: should add sanity checking that ranges
//     are not overlapping.   
//   - our overall vm scheme is janky since still has assumptions encoded.
//   - in real life would want more control over region attributes.
static inline procmap_t procmap_default_mk(unsigned dom) {
    enum { MB = 1024 * 1024 };

    procmap_t p = {};
    procmap_push(&p, pr_ent_mk(0x20000000, MB, MEM_DEVICE, dom));
    procmap_push(&p, pr_ent_mk(0x20100000, MB, MEM_DEVICE, dom));
    procmap_push(&p, pr_ent_mk(0x20200000, MB, MEM_DEVICE, dom));

    // make sure entire program fits on 1 page: when done, will split
    // this onto two pages and mark the code as RO.
    extern char __prog_end__[];
    assert((uint32_t)__prog_end__ <= MB);
    procmap_push(&p, pr_ent_mk(0x00000000, MB, MEM_RW, dom));

    // heap
    char *start = kmalloc_heap_start();
    char *end = kmalloc_heap_end();
    unsigned nbytes = end - start;

    // for today: check that heap starts at 1MB.
    assert(start == (void*)MB);
    // and that its 1MB big.
    demand(nbytes == MB, "nbytes = %d\n", nbytes);

    procmap_push(&p, pr_ent_mk(0x00100000, MB, MEM_RW, dom));

    // the two hardcoded stacks we use.
    procmap_push(&p, pr_ent_mk(INT_STACK_ADDR-MB, MB, MEM_RW, dom));
    procmap_push(&p, pr_ent_mk(STACK_ADDR-MB, MB, MEM_RW, dom));

    return p;
}
#endif
