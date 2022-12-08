#include "include/gpio.h"
#include "include/clock.h"
#include "sw-uart.h"

int mymain() {
    enable_clock();
    gpio_reset();
    //release reset on IO_BANK0
    gpio_set_output(0);
    gpio_set_on(0);
    gpio_set_input(1);
    sw_uart_t uart = sw_uart_init(0, 1, 300);

    gpio_set_output(25); 
        //output disable

    while(1) {
        gpio_set_on(25);
        // Second
        delay_ms(1000);
        gpio_set_off(25);
        sw_uart_putk(&uart, "Hello World");
        //seconds_start();
        delay_ms(1000);
    }
    return 0;

}