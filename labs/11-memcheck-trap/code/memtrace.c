// engler: cs240lx simple system to do memory tracing:
//
//  1. traps on each load or store to a range of memory.
//  2. calls a client supplied handler with all the registers
//     and the address of the fault.
//  3. client code can modify the provided registers to change
//     behavior and/or skip the faulting instruction.
// 
// makes it easy to build a variety of tools.
// 
// notes:
//   - i think pull the single step part out: that is pretty clean.
//     run_one_inst(regs, handler);
//     trap_memory(handler);
//
//   - in general our subsystems have bundled too much functionality,
//     which makes them a pain to compose.  we want to turn the code 
//     more inside out.  but then the 
//   - for pinning: we could give a way to tag an entry with a new domain.
//     you'd go in and change it in the pt and in the TLB for consistency.
#include "memtrace-internal.h"
#include "breakpoint.h"

enum { memtrace_dom = 2 };

// default: print stuff.
int memtrace_verbose_p = 1;

// total number of faults.
static volatile unsigned nfaults = 0;

// pointer to client handler to call on each domain fault.
//   - currently only have a single handler: easy to allow
//     multiple.
static memtrace_fn_t client_handler;

// pre-computed domain reg value specifying the
// domains to set for trapping heap access
static unsigned trap_heap_access;
// pre-computed domain reg value specifying the 
// domains to set for not trapping heap accesses
static unsigned no_trap_heap_access;

// remove access to <memtrace_dom> so get faults.
void memtrace_trap_enable(void) {
    staff_domain_access_ctrl_set(trap_heap_access);
}
// allow access to <memtrace_dom> so don't get faults.
void memtrace_trap_disable(void) {
    staff_domain_access_ctrl_set(no_trap_heap_access);
}

// handle the single-step mis-match exception: 
//   1. disable the mismatch breakpoint
//   2. re-enable trapping
//   3. jump back to super mode [easy to make more flexible]
//
// todo:
//   1. check that we did indeed get a mismatch and 
//      not something else.
//   2. use watchpoints to handle LDM etc.
static void 
prefetch_abort_memtrace(uint32_t *regs, uint32_t spsr, uint32_t pc) {
    if(!was_debug_fault())
        panic("was not a debug fault: %b\n", 
            prefetch_print_reason(pc));

    assert(mode_get(spsr) == USER_MODE);
    assert(regs[MCHK_CPSR_OFF] == spsr);

    mem_debug("mismatch about to resume back to: %x!\n", pc);

    // switch back to privileged mode and re-enable trapping.
    regs[MCHK_CPSR_OFF] = mode_set(spsr, SUPER_MODE);
    brkpt_disable0();
    memtrace_trap_enable();
    switchto(regs);
}

// handle a domain fault so that we can trace each load/store:
//   1. disable trapping
//   2. call client code.
//
// if client wants load/store executed, then we side-step the need
// to understand instruction semantics by running in single-step 
// mode:
//   1. set a mismatch exception on the load/store instruction
//   2. change the mode to USER (so that mismatch works)
//   3. jump back --- after the instruction completes the prefetch 
//      abort will run.
static void 
data_abort_memtrace(uint32_t *regs, uint32_t spsr, uint32_t pc) {
    uint32_t reason = data_abort_reason();
    uint32_t fault_addr = data_abort_addr();

    if(reason != DOMAIN_SECTION_FAULT)
        panic("was not a debug fault: %b\n", 
                data_abort_print_reason(pc));

    mem_debug("got a domain section fault at pc=%x: address %x\n", pc, fault_addr);
    assert(regs[MCHK_PC_OFF] == pc);
    assert(regs[MCHK_CPSR_OFF] == spsr);
        
    assert(spsr == spsr_get());
    assert(mode_get(spsr) == SUPER_MODE);

    nfaults++;
    mode_get_sp_lr(regs);
    assert(regs[MCHK_CPSR_OFF] == spsr);

    // if we have a client handler and it says to skip
    memtrace_trap_disable();
    if(client_handler(pc, fault_addr, data_fault_from_ld())) {
        mem_debug("set a mismatch on pc=%x [regs[15]=%x]\n", pc, regs[15]);
        brkpt_mismatch_set0_raw(pc);
        regs[16] = mode_set(regs[16], USER_MODE);
    }
    switchto(regs);
    not_reached();
}

void memtrace_on(void) {
    demand(client_handler, not initialized!);
    assert(mmu_is_enabled());
    assert(mode_is_super());

    // enable debugging so we can do single-stepping.
    cp14_enable();
    brkpt_disable0();

    // trap all heap access.
    memtrace_trap_enable();
}

void memtrace_off(void) {
    // shouldn't happen.
    demand(client_handler, not initialized!);
    assert(mmu_is_enabled());

    // stop trapping accesses.
    memtrace_trap_disable();
}

// in order to make sanity checking for the lab easier we 
// we force:
//   - a single fixed domain 
//   - a single fixed region
//   - must trace the entire segment.
// these restrictions are obviously ridiculous.
//
// if you have multiple checkers that check disjoint regions
// a speed hack would be to use different domains.
int memtrace_trap_region(procmap_t *pmap, void *addr, unsigned nbytes) {
    assert(nbytes == 1024*1024);

    // just for simplicity: assert we have a single domain id 
    assert(pmap->dom_ids == (1<<1));
    // [implied by above]: the memtrace_dom is avail.
    assert(!bit_isset(pmap->dom_ids, memtrace_dom));

    // get rid of the domains and go back to the old procmap.
    pr_ent_t *e = procmap_lookup(pmap, addr);
    e->dom = memtrace_dom;

    // precompute the domain register values: might be 
    // easier to just do rmw of what's in there. 
    trap_heap_access = dom_perm(pmap->dom_ids, DOM_client);
    pmap->dom_ids |= 1<<memtrace_dom;
    no_trap_heap_access = dom_perm(pmap->dom_ids, DOM_client);

    return memtrace_dom;
}

// start tracing:
//  - setup our exception handlers;
//  - change permissions so that we can access memory from user level.
//  - start pinned virtual memory.
void memtrace_init(procmap_t *pmap, memtrace_fn_t h, uint32_t vecs[]) {
    demand(!client_handler, for today: should only call once);
    client_handler = h;

    // for today's lab: make sure we always track something.
    if(!bit_isset(pmap->dom_ids, memtrace_dom))
        panic("calling memtrace_init but not tracing any region\n");

    // install our handlers [note: if <vecs> doesn't interact
    // with full_except, this won't have any effect.]
    full_except_set_prefetch(prefetch_abort_memtrace);
    full_except_set_data_abort(data_abort_memtrace);

    // start: when MMU is turned on we will start
    // trapping!
    staff_pin_mmu_on_v(pmap, vecs);

    assert(mmu_is_enabled());
}
