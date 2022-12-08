#include "../include/gpio.h"
#include <stdint.h>

uint32_t gpio_is_reset() {
    return (GET32(RESET_DONE) >> 5 & 0x1);
}

void gpio_reset() {
    PUT32(RESETS_RESET_CLR, 1 << RESET_IO_BANK0);
    while(!gpio_is_reset()) {};
}

// TODO: Checkout Section 2.19.2 and page 46
void gpio_set_input(uint32_t pin) {
    if (pin > NUM_GPIOS) {
        return;
    } 
    // 0. Get offset
    // 1. Function Select
    // 2. Set as output/input
}

uint32_t gpio_read(uint32_t pin) {
    if (pin > NUM_GPIOS) {
        return 0;
    } 
    return (GET32(GPIO_IN) >> pin) & 1;
}

// TODO: Checkout Section 2.19.2
void gpio_set_output(uint32_t pin) {
    // Enable as output
    if (pin > NUM_GPIOS) {
        return;
    } 
    // 0. Get offset
    // 1. Output enable clear
    // 2. Clear output
    // 3. Function select SIO use IO_BANK_BASE_CTRL
    // 4. Set as output 
}

// TODO: pg.42 in rp2040 datasheet
void gpio_set_on(uint32_t pin) {
    return;
}

// TODO: pg.42 in rp2040 datasheet
void gpio_set_off(uint32_t pin) {
    return;
}

//TODO: Only If you're doing the hw-uart lab do you have to write the gpio_set_function.
// You can comeback to this later if you want
void gpio_set_function(uint32_t pin, gpio_func_t func) {
    return;
}