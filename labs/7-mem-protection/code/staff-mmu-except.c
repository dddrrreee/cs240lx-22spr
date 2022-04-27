#include "rpi.h"
#include "mmu.h"
#include "libc/bit-support.h"
#include "armv6-debug-impl.h"

// b4-20
enum {
    SECTION_XLATE_FAULT = 0b00101,
    SECTION_PERM_FAULT = 0b1101,
};



// this is called by reboot: we turn off mmu so that things work.
void reboot_callout(void) {
    if(mmu_is_enabled())
        staff_mmu_disable();
}

void prefetch_abort_vector(unsigned lr) {
    unsigned ifsr = cp15_ifsr_get();
    unsigned reason = bits_get(ifsr, 0, 3) | bit_get(ifsr, 10) << 10;
    uint32_t ifar = cp15_ifar_get();

    switch(reason) {
    case SECTION_XLATE_FAULT:
        panic("ERROR: attempting to run unmapped addr %p [reason=%b]\n", ifar, reason);
    case SECTION_PERM_FAULT:
        panic("ERROR: attempting to run addr %p with no permissions [reason=%b]\n", ifar, reason);
        break;
    default:
        panic("unexpected reason %b\n", reason);
    }

    not_reached();
}

void data_abort_vector(unsigned lr) {
    // b4-43
    unsigned dfsr = cp15_dfsr_get();
    unsigned reason = ( (dfsr >> 4) & (1<<4)) |  (dfsr & 0b1111);

    reason = bits_get(dfsr, 0, 3) | bit_get(dfsr, 10) << 10;

    // b4-44
    unsigned fault_addr;
    asm volatile("MRC p15, 0, %0, c6, c0, 0" : "=r" (fault_addr));

    switch(reason) {
    // b4-20
    case SECTION_XLATE_FAULT:
        panic("section xlate fault: %x\n", fault_addr);
    case SECTION_PERM_FAULT:
        panic("section permission fault: %x", fault_addr);
    default: 
        panic("unknown reason %b\n", reason);
    }

    not_reached();
}
