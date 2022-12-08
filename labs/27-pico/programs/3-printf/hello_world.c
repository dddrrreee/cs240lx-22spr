#include "include/gpio.h"
#include "include/clock.h"
#include "include/cstart.h"
#include "include/sw-uart.h"
#include "printf.h"

#define ACTUAL_SECOND 1000000

int mymain() {
    enable_clock();
    gpio_reset();

    sw_uart_t uart = sw_uart_init(0, 1, 300);
    // Initialize printf

    gpio_set_output(25); 

    while(1) {
        gpio_set_on(25);
        delay_us(ACTUAL_SECOND);
        printf("Hello World\n");
        gpio_set_off(25);
        delay_us(ACTUAL_SECOND);
    }
    return 0;

}