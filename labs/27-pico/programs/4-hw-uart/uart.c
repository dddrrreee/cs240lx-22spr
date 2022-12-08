#include "include/gpio.h"
#include "include/uart.h"
#include "include/integer-math.h"
#include "include/clock.h"

void reset_periph(uint32_t loc) {
    PUT32(RESET + CLR_OFFSET, 1 << loc);
    while(!((GET32(RESET_DONE) >> loc) & 0x1)) {;}
}

// Setup hardware uart
void uart_init(hw_uart_t *uart) {
    // 0. Setup UART Pins to correct function

    if (uart->index != UART0 && uart->index != UART1) {
        return;
    }
    reset_periph(RESET_UART0);

    // 1. Setup UART Speed via clk_peri
    uint32_t baud_rate_divisor = udiv(CLK_PERI_DEFAULT_SPEED, 16 * uart->baud_rate);
    // Careful with overflow
    uint32_t baud_rate_frac = udiv(CLK_PERI_DEFAULT_SPEED * 100, 16 * uart->baud_rate) - (baud_rate_divisor * 100);
    uint32_t fbrd = udiv((baud_rate_frac * 64), 100);
    // 2. Set Integer and Fraction BaudRate pg. 452

    // 3. Setup uart->index with 8n1 (pg 456)

    // 4. Enable fifos (pg 456)

    // 5. Enable UART, TRX, and RX pg 457
}

// 1 = at least one byte on rx queue, 0 otherwise
static int uart_can_getc(void) {
    return 0;
}

// returns one byte from the rx queue, if needed
// blocks until there is one.
int uart_getc(void) {
	return 0;
}

// 1 = space to put at least one byte, 0 otherwise.
int uart_can_putc(hw_uart_t* uart) {
    return !((GET32(uart->index + UART_FR_OFFSET) >> 5) & 1);
}

// put one byte on the tx qqueue, if needed, blocks
// until TX has space.
void uart_putc(hw_uart_t* uart, unsigned char c) {
    while(!uart_can_putc(uart)){;}
    PUT32(uart->index + UART_DATA_OFFSET, c);
}


// simple wrapper routines useful later.
// a maybe-more intuitive name for clients.
int uart_has_data(void) {
    return uart_can_getc();
}

int uart_getc_async(void) { 
    if(!uart_has_data())
        return -1;
    return uart_getc();
}