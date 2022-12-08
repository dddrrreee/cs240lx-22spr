#include "include/gpio.h"
#include "include/clock.h"
#include "include/uart.h"
#include "include/printf.h"
#define RESETS_RESET_CLR       (RESETS_BASE+0x0+0x3000)
#define RESETS_BASE                 0x4000C000
#define RESETS_RESET_DONE_RW   (RESETS_BASE+0x8+0x0000)

int mymain() {
    enable_clock();
    // Sets up the XOSC clock for peripherals
    PUT32(CLK_PERI_CTRL, (1 << 11) | (0x4 << 5));
    gpio_reset();
    gpio_set_output(25);

    hw_uart_t uart;
    uart.tx = 12;
    uart.rx = 13;
    uart.baud_rate = 115200;
    uart.index = UART0;
    uart_init(&uart);
    init_putchar(&uart);

    while(1) {
        gpio_set_on(25);
        delay_ms(1000);
        gpio_set_off(25);
        printf("Hello World\n");
        delay_ms(1000);
    }
    return 0;

}