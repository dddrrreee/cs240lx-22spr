// put your code here.
//
#include "rpi.h"
#include "libc/bit-support.h"

// has useful enums and helpers.
#include "vector-base.h"
#include "pinned-vm.h"
#include "pinned-vm-asm.h"
#include "mmu.h"
#include "procmap.h"

// cp_asm(lockdown_index, p15, 5, c15, c4, 2); //get/set for lockdown index reg
// cp_asm(lockdown_attr, p15, 5, c15, c7, 2); //get/set for lockdown attr reg
// cp_asm(lockdown_index, p15, 5, c15, c5, 2); //get/set for lockdown VA reg
// cp_asm(lockdown_index, p15, 5, c15, c6, 2); //get/set for lockdown PA reg


// generate the _get and _set methods.
// (see asm-helpers.h for the cp_asm macro 
// definition)
// arm1176.pdf: 3-149

// do a manual translation in tlb:
//   1. store result in <result>
//   2. return 1 if entry exists, 0 otherwise.
int tlb_contains_va(uint32_t *result, uint32_t va) {
    // 3-79
    assert(bits_get(va, 0,2) == 0);

    unimplemented();
}

// map <va>-><pa> at TLB index <idx> with attributes <e>
void pin_mmu_sec(unsigned idx,  
                uint32_t va, 
                uint32_t pa,
                pin_t e) {

    demand(idx < 8, lockdown index too large);
    // lower 20 bits should be 0.
    demand(bits_get(va, 0, 19) == 0, only handling 1MB sections);
    demand(bits_get(pa, 0, 19) == 0, only handling 1MB sections);

    if(va != pa)
        panic("for today's lab, va (%x) should equal pa (%x)\n",
                va,pa);

    debug("about to map %x->%x\n", va,pa);


    // these will hold the values you assign for the tlb entries.
    uint32_t x, va_ent, pa_ent, attr;

    // put your code here.
	// assign which lockdown index we will use for future register accesses
	lockdown_index_set(idx);

	// asid can only occupy bottom 8 bits
	demand(e.asid < 256, "ASID must be less than 256\n");
	//setup va ent and write to lockdown_va_reg
	va_ent = 0;
	va_ent |= (0xff & e.asid);
	va_ent |= ((e.G & 0b1) << 9);
	va_ent |= (va & 0xfffff) << 12;
	lockdown_va_set(va_ent);

	//set mem attr with lockdown_attr_set
	demand(e.dom < 16, "Domain must be less than 16.\n");
	attr = 0;
	attr |= (e.mem_attr & 0xf);
	attr |= (e.dom & 0xf) << 7;
	lockdown_attr_set(attr);

	//set pa ent and write to lockdown_pa_reg
	demand(e.pagesize < 4, "Invalid pagesize value\n");
	demand(mem_perm_islegal(e.AP_perm), "Illegal perms for memory ent.");
	pa_ent = 0;
	pa_ent |= (e.AP_perm & 0b111) << 1;
	pa_ent |= (e.pagesize & 0b11) << 6;
	pa_ent |= (pa & 0xfffff) << 12;
	lockdown_pa_set(pa_ent);

    if((x = lockdown_va_get()) != va_ent)
        panic("lockdown va: expected %x, have %x\n", va_ent,x);
    if((x = lockdown_pa_get()) != pa_ent)
        panic("lockdown pa: expected %x, have %x\n", pa_ent,x);
    if((x = lockdown_attr_get()) != attr)
        panic("lockdown attr: expected %x, have %x\n", attr,x);
}


// check that <va> is pinned.  
void pin_check_exists(uint32_t va) {
    if(!mmu_is_enabled())
        panic("XXX: i think we can only check existence w/ mmu enabled\n");

    uint32_t r;
    if(tlb_contains_va(&r, va)) {
        pin_debug("success: TLB contains %x, returned %x\n", va, r);
        assert(va == r);
    } else
        panic("TLB should have %x: returned %x [reason=%b]\n", 
            va, r, bits_get(r,1,6));
}

// TLB pin all entries in procmap <p>
// very simpleminded atm.
void pin_procmap(procmap_t *p) {
    for(unsigned i = 0; i < p->n; i++) {
        pr_ent_t *e = &p->map[i];
        assert(e->nbytes == MB);

        switch(e->type) {
        case MEM_DEVICE:
                pin_mmu_sec(i, e->addr, e->addr, pin_mk_device(e->dom));
                break;
        case MEM_RW:
        {
                // currently everything is uncached.
                pin_t g = pin_mk_global(e->dom, perm_rw_priv, MEM_uncached);
                pin_mmu_sec(i, e->addr, e->addr, g);
                break;
        }
        case MEM_RO: panic("not handling\n");
        default: panic("unknown type: %d\n", e->type);
        }
    }
}


// turn the pinned MMU system on.
//    1. initialize the MMU (maybe not actually needed): clear TLB, caches
//       etc.  if you're obsessed with low line count this might not actually
//       be needed, but we don't risk it.
//    2. allocate a 2^14 aligned, 0-filled 4k page table so that any nonTLB
//       access gets a fault.
//    3. set the domain privileges (to DOM_client)
//    4. set the exception handler up using <vector_base_set>
//    5. turn the MMU on --- this can be much simpler than the normal
//       mmu procedure since it's never been on yet and we do not turn 
//       it off.
//    6. profit!
void pin_mmu_on(procmap_t *p) {
    assert(!mmu_is_enabled());

    // we have to clear the MMU before setting any entries.
    staff_mmu_init();
    pin_procmap(p);

    // 0 filled page table to get fault on any lookup.
    void *null_pt = kmalloc_aligned(4096 * 4, 1 << 14);
    demand((unsigned)null_pt % (1<<14) == 0, must be 14 bit aligned);

    // XXX: invalidate TLB routines don't seem to invalidate the 
    // pinned entries.

    // right now we just have a single domain?
    // domain_access_ctrl_set(DOM_client << kern_dom*2);
	staff_domain_access_ctrl_set(DOM_client << kern_dom*2);

    // install the default vectors.
    extern uint32_t default_vec_ints[];
    vector_base_set(default_vec_ints);

    pin_debug("about to turn on mmu\n");
    staff_mmu_on_first_time(1, null_pt);
    assert(mmu_is_enabled());
    pin_debug("enabled!\n");

    // can only check this after MMU is on.
    pin_debug("going to check entries are pinned\n");
    for(unsigned i = 0; i < p->n; i++)
        pin_check_exists(p->map[i].addr);
}
