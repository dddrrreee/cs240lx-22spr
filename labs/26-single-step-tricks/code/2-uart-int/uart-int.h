#ifndef __UART_INT_H__
#define __UART_INT_H__

#include "rpi.h"

#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_IRQ         0x20215000
#define IRQ_ENABLE1     0x2000B210
#define IRQ_DISABLE1    0x2000B21C

// This function turns on interrupts
// for AUX_INT for mini-uart.
void uart_int_init_on();

// This function turns off interrupts
// for AUX_INT for mini-uart.
void uart_int_init_off();

// Checker whether this is an 
// interrupt pending for mini-uart.
int is_int_pending();

// Turn on interrupts on RX for mini-uart.
void uart_rx_int_init(void);

#endif