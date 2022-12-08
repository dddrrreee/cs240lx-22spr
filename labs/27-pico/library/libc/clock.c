#include "../include/gpio.h"
#include "../include/clock.h"

// Clock is 5.8 times slower than expected
#define US_CYCLES 11
#define SCALING_FACTOR 6

// TODO: Cannot have cycles_start nested and cannot be more than 16 seconds
void seconds_start() {
    PUT32(STK_CSR, 4);
    PUT32(STK_RVR, 12000000 - 1);
    // Clear Current Value register
    PUT32(STK_CVR, 0);
    PUT32(STK_CSR, 5);
    while(!((GET32(STK_CSR) >> 16) & 1)) {;}
}


// Clock will run at 12 MHZ
void enable_clock() {
    // frequency_mhz * (1000 / 256);
    uint32_t delay = 47;
    PUT32(XOSC_CTRL, 0xaa0);
    PUT32(XOSC_STARTUP, delay);
    PUT32(XOSC_CTRL + SET_OFFSET, 0xfab << 12);

    // Wait till ready
    while (!(GET32(XOSC_STATUS) >> 31));
    // Setup clk_ref so you can setup clock_sys
    PUT32(CLK_REF_CTRL, 0x2);
    PUT32(CLK_SYS_CTRL, 0x0);
}

void delay_us(uint32_t us) {
    //TODO: Get working
    //uint32_t scaled_us = us / SCALING_FACTOR;
    micro_seconds_start();
    for (uint32_t i = 0; i < us; i++) {
        while(!micro_second_end()) {;}
    }
}


// TODO: Cannot have cycles_start nested and cannot be more than 16 seconds
void micro_seconds_start() {
    PUT32(STK_CSR, 0);
    PUT32(STK_RVR, US_CYCLES);
    // Clear Current Value register
    PUT32(STK_CVR, 0);
    PUT32(STK_CSR, 1);
}

uint32_t micro_second_end() {
    //uint32_t cur_val = GET32(STK_CVR) & (0x7fffff);
    uint32_t hit_zero = (GET32(STK_CSR) >> 16) & 1;
    return !hit_zero;
}