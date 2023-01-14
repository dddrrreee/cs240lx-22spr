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
    // p268
    volatile uint32_t *fn = (void*)  (0x40014000 + 0x4 + 8 * pin);
    *fn = 5;

    // 0. Get offset
    volatile uint32_t *oe = (void*)GPIO_OE;
    uint32_t v = *oe;
    // clear
    v &= ~ (1 << pin);
    // set
    v |= 1 << pin;
    // write it out.
    *oe = v;


    
    // 1. Output enable clear
    // 2. Clear output
    // 3. Function select SIO use IO_BANK_BASE_CTRL
    // 4. Set as output 
}

// TODO: pg.42 in rp2040 datasheet
void gpio_set_on(uint32_t pin) {
    volatile uint32_t *out_set = (void*) 0xd0000000 + 0x14;
    *out_set  = 1 << pin;
}

// TODO: pg.42 in rp2040 datasheet
void gpio_set_off(uint32_t pin) {
    volatile uint32_t *out_clr = (void*) 0xd0000000 + 0x18;
    *out_clr  = 1 << pin;
}

//TODO: Only If you're doing the hw-uart lab do you have to write the gpio_set_function.
// You can comeback to this later if you want
void gpio_set_function(uint32_t pin, gpio_func_t func) {
    return;
}
