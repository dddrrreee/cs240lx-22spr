#include "rpi.h"
#include "uart-int.h"

void uart_int_init_on(){
    dev_barrier();
    unimplemented();
    // Enable AUX_INT interrupts by setting the correct bit in IRQ_ENABLE1.
    dev_barrier();
}

void uart_int_init_off(){
    dev_barrier();
    unimplemented();
    // Disable AUX_INT interrupts by setting the correct bit in IRQ_DISABLE1.
    dev_barrier();
}

int is_int_pending(){
    unimplemented();
    // Check whether we have a mini-uart interrupt pending by checking 
    // the correct bit from AUX_MU_IIR_REG.
    return 0;
}

void uart_rx_int_init(void) {
    dev_barrier();
    // Set the correct bit in AUX_MU_IER_REG and turn on 
    // RX interrupts.
    // Errata!! The RX and TX bits are fipped.
    unimplemented();
    dev_barrier();
}