// should have the same output as 1-brkpt-test.c

// simple breakpoint test:
//  1. set a single breakpoint on <foo>.
//  2. in exception handler, make sure it's a debug exception and disable.
//  3. if that worked, do 1&2 <n> times to make sure works.
#include "rpi.h"
#include "armv6-debug-impl.h"
#include "vector-base.h"


extern void    start_function(uint32_t function);
extern uint8_t  kernel_debug;

// total number of faults.
static volatile int n_faults = 0;
static volatile int n_undefined_faults = 0;

void prefetch_abort_vector(unsigned lr) {
    if(!was_brkpt_fault())
        panic("should only get a breakpoint fault\n");
    n_faults++;
    cp14_bcr0_mismatch();
    cp14_bvr0_set(lr);
}

void undefined_instruction_vector_debug(unsigned pc, unsigned* sp){
    uint32_t* pc_pointer = (uint32_t*) pc;
    uint32_t true_reg = (*pc_pointer >> 12) & 0xf;

    printk("**************************\n");
    printk("INSTRUCTION:            %x\n", *pc_pointer);
    printk("ADDR INSTRUCTION:       %x\n", pc_pointer);
    printk("TRUE REG:               %d\n", true_reg);
    printk("VAL AT TRUE REG:        %x\n", sp[true_reg]);
    
    uint32_t val = sp[true_reg];
    uint32_t execute_single_addr[2];

    // Implement the trampoline that emulates the 
    // undefined instruction using register r0.
    unimplemented();

    uint32_t (*fp)(uint32_t) = (typeof(fp))execute_single_addr;

    val = fp(val);
    sp[true_reg] = val;
    n_undefined_faults++;
}

void foo(int x) {
    trace("running foo: %d\n", x);
}

void single_step_function(void){    
    foo(1);
    foo(2);

    cp14_bcr0_enable();
    assert(cp14_bcr0_is_enabled());

    foo(3);
    foo(4);

    cp14_bcr0_disable();
    assert(!cp14_bcr0_is_enabled());

    foo(5);
    foo(6);
    
}


void notmain(void) {
    // 1. install exception handlers.
    vector_base_set(&kernel_debug);

    // 2. enable the debug coprocessor.
    cp14_enable();

    // just started, should not be enabled.
    assert(!cp14_bcr0_is_enabled());

    // 2. enable the debug coprocessor.
    // and set a breakpoint mismatch. 
    enable_breakpoint((uint32_t) foo);
    assert(cp14_bcr0_is_enabled());

    // Start function puts us in user mode
    // for breakpoint mismatch. The function 
    // puts us in super mode when we return. 
    start_function((uint32_t)single_step_function);

    output("Stepped through %d instructions\n", n_faults);
    output("Single Stepped Through %d kernel instructions\n", n_undefined_faults);
}


