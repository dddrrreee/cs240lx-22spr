#ifndef __UART_H__
#define __UART_H__

#include "gpio.h"
#define UART0 0x40034000
#define UART1 0x40038000

enum {
    UART_DATA_OFFSET = 0x00,
    UART_RSR_OFFSET = 0x004,
    UART_FR_OFFSET = 0x018,
    UART_IBRD_OFFSET = 0x024, // int baud rate register
    UART_FBRD_OFFSET = 0x028, // fractional baud rate register
    UART_LCR_OFFSET = 0x02c, // line control register
    UART_CTRL_OFFSET = 0x030, 
};

typedef struct {
    uint32_t tx,rx;
    uint32_t baud_rate;
    uint32_t index;
} hw_uart_t;

//  - you will need memory barriers, use <dev_barrier()>
//
//  later: should add an init that takes a baud rate.
void uart_init(hw_uart_t * uart);

// returns one byte from the rx queue, if needed
// blocks until there is one.
int uart_getc();

// 1 = space to put at least one byte, 0 otherwise.
int uart_can_putc(hw_uart_t* uart);

// put one byte on the tx qqueue, if needed, blocks
// until TX has space.
void uart_putc(hw_uart_t* uart,unsigned char c);

// a maybe-more intuitive name for clients.
int uart_has_data();

int uart_getc_async();
#endif