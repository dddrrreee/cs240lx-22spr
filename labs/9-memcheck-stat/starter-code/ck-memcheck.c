// starter code for trivial heap checking using interrupts.
#include "rpi.h"
#include "rpi-internal.h"
#include "timer-interrupt.h"
#include "ckalloc.h"
#include "rpi-interrupts.h"


// you'll need to pull your code from lab 2 here so you
// can fabricate jumps
// #include "armv6-insts.h"

// used to check initialization.
static volatile int init_p, check_on;

// allow them to limit checking to a range.  for simplicity we 
// only check a single contiguous range of code.  initialize to 
// the entire program.
static uint32_t 
    start_check = (uint32_t)&__code_start__, 
    end_check = (uint32_t)&__code_end__,
    // you will have to define these functions.
    start_nocheck = (uint32_t)ckalloc_start,
    end_nocheck = (uint32_t)ckalloc_end;


static int in_range(uint32_t addr, uint32_t b, uint32_t e) {
    assert(b<e);
    return addr >= b && addr < e;
}

// if <pc> is in the range we want to check and not in the 
// range we cannot check, return 1.
int (ck_mem_checked_pc)(uint32_t pc) {
	if (in_range(pc, start_check, end_check) && !in_range(pc, start_nocheck, end_nocheck))
		return 0;
	return 1;
}

// useful variables to track: how many times we did 
// checks, how many times we skipped them b/c <ck_mem_checked_pc>
// returned 0 (skipped)
static volatile unsigned checked = 0, skipped = 0;

unsigned ck_mem_stats(int clear_stats_p) { 
    unsigned s = skipped, c = checked, n = s+c;
    printk("total interrupts = %d, checked instructions = %d, skipped = %d\n",
        n,c,s);
    if(clear_stats_p)
        skipped = checked = 0;
    return c;
}

// note: lr = the pc that we were interrupted at.
// longer term: pass in the entire register bank so we can figure
// out more general questions.
void ck_mem_interrupt(uint32_t pc) {

    // we don't know what the user was doing.
    dev_barrier();

	if (!ck_mem_checked_pc(pc)) {
		skipped++;
		return;
	}
	trace("Within bounds, going to check.\n");
	ck_find_leaks(0);
	checked++;

    // we don't know what the user was doing.
    dev_barrier();
}

void interrupt_vector(unsigned pc) {
	dev_barrier();
	system_disable_interrupts();
    unsigned pending = GET32(IRQ_basic_pending);

    // if this isn't true, could be a GPU interrupt (as discussed in Broadcom):
    // just return.  [confusing, since we didn't enable!]
    if((pending & RPI_BASIC_ARM_TIMER_IRQ) == 0)
        return;

    // Checkoff: add a check to make sure we have a timer interrupt
    // use p 113,114 of broadcom.
	unsigned timer_int = GET32(arm_timer_MaskedIRQ);
	if (!timer_int)
		panic("interrupted, but wasnt timer?\n");
	
	ck_mem_interrupt(pc);

	PUT32(arm_timer_IRQClear, 1);

    // asm volatile("stm %0, {lr}^" : "=r"(shadow_lr)); 

    /*
     * We do not know what the client code was doing: if it was touching a 
     * different device, then the broadcom doc states we need to have a
     * memory barrier.   NOTE: we have largely been living in sin and completely
     * ignoring this requirement for UART.   (And also the GPIO overall.)  
     * This is probably not a good idea and we should be more careful.
	 * 
     */
	system_enable_interrupts();
    dev_barrier();  
	
}


// do any interrupt init you need, etc.
void ck_mem_init(void) { 
    assert(!init_p);
    init_p = 1;

    assert(in_range((uint32_t)ckalloc, start_nocheck, end_nocheck));
    assert(in_range((uint32_t)ckfree, start_nocheck, end_nocheck));
    assert(!in_range((uint32_t)printk, start_nocheck, end_nocheck));

	//setup interrupt handlers
	int_init();

	// setup timer interrupt
	timer_interrupt_init(0x100);
}

// only check pc addresses [start,end)
void ck_mem_set_range(void *start, void *end) {
    assert(start < end);
	start_check = (uint32_t)start;
	end_check = (uint32_t)end;
}

// maybe should always do the heap check at the begining
void ck_mem_on(void) {
    assert(init_p && !check_on);
    check_on = 1;

	system_enable_interrupts();
}

// maybe should always do the heap check at the end.
void ck_mem_off(void) {
    assert(init_p && check_on);

	system_disable_interrupts();
    check_on = 0;
}
