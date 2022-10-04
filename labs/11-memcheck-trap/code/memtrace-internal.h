#ifndef __MEMTRACE_INTERNAL_H__
#define __MEMTRACE_INTERNAL_H__
#include "rpi.h"
#include "cpsr-util.h"
#include "mmu.h"
#include "memtrace.h"
#include "pinned-vm.h"
#include "procmap.h"

// offsets used for registers in <regs> array.
// exception trampoline should save in these
// locations.
enum {
    MCHK_SP_OFF = 13,
    MCHK_LR_OFF = 14,
    MCHK_PC_OFF = 15,
    MCHK_CPSR_OFF = 16
};

// some internal routines useful for testing code to call.

// run <fp> at user level: should not return.
// used for testing to see that the memory permissions from the 
// pinnned vm let us load and store at user level.
void run_fn_at_user_level(void (*fp)(void)) __attribute__((noreturn));

// defined in staff-exception-asm.S
//
// get the sp from privileged mode <cpsr>
// by switching to that mode, reading it,
// and switching back.
uint32_t mode_get_sp_asm(uint32_t cpsr);

// defined in staff-exception-asm.S
//
// get the lr from mode <cpsr>
uint32_t mode_get_lr_asm(uint32_t cpsr);

static inline void mode_get_sp_lr(uint32_t *regs) {
    uint32_t mode_cpsr = regs[MCHK_CPSR_OFF];

    if(mode_get(mode_cpsr) == USER_MODE)
        panic("cannot handle user level\n");
    regs[MCHK_LR_OFF] = mode_get_lr_asm(mode_cpsr);
    regs[MCHK_SP_OFF] = mode_get_sp_asm(mode_cpsr);
}

// swiich to privileged mode using <regs>
void switchto_priv_asm(uint32_t *regs)  __attribute__((noreturn));
// swiich to user mode using <regs>
void switchto_user_asm(uint32_t *regs)  __attribute__((noreturn));

// figure out if we are jumping to user or
// a privileged mode and use the right 
// context switching routine.
static inline void __attribute__((noreturn)) 
switchto(uint32_t *regs)  
{
    if(mode_get(regs[MCHK_CPSR_OFF]) == USER_MODE)
        switchto_user_asm(regs);
    else
        switchto_priv_asm(regs);
}

/******************************************************
 * the following are just pulled in from our 
 * debug and virtual memory labs.
 *
 * you can (should!) use your own if you have time.
 */

// b4-43: 140e: lab 12-vm.
uint32_t data_abort_reason(void);
// b4-44 140e: lab 12-vm.
uint32_t data_abort_addr(void);

// reason for prefetch abort: 140e: lab 9 debug hw
uint32_t prefetch_reason(void);
// address of prefetch abort: 140e: lab 9 debug hw
uint32_t prefetch_addr(void);

// from the debug lab [datafault_from_ld]
unsigned data_fault_from_ld(void);

// from debug lab: 13-11: 
//   - check dscr register to see if breakpoint
//     occured.
unsigned was_debug_fault(void);

// disable breakpoint 0 [bcr0 register]
void brkpt_disable0(void);

// b4-20
enum {
    SECTION_XLATE_FAULT = 0b00101,
    SECTION_PERM_FAULT = 0b1101,
    DOMAIN_SECTION_FAULT = 0b1001,
};

// print and return reason for a prefetch abort: 
// used for error handling.  very incomplete.
static inline uint32_t 
prefetch_print_reason(uint32_t pc) { 
    uint32_t reason = prefetch_reason();
    uint32_t ifar = prefetch_addr();

    switch(reason) {
    case SECTION_XLATE_FAULT:
        output("pc=%x: attempted to run unmapped addr %p [reason=%b]\n", pc, ifar, reason);
        break;
    case SECTION_PERM_FAULT:
        output("pc=%x: attempted to run addr %p with no permissions [reason=%b]\n",
            pc, ifar, reason);
        break;
    default: 
        panic("unexpected reason %b\n", reason);
    }

    return reason;
}

static inline uint32_t 
data_abort_print_reason(uint32_t pc) {
    uint32_t reason = data_abort_reason();
    uint32_t fault_addr = data_abort_addr();

    switch(reason) {
    // b4-20
    case SECTION_XLATE_FAULT:
        panic("section xlate fault: %x\n", fault_addr);
    case SECTION_PERM_FAULT:
        panic("section permission fault: pc=%x: addr=%x", pc,fault_addr);
    default: panic("unknown reason %b\n", reason);
    }
}
#endif
