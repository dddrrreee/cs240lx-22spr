#include "include/gpio.h"
#include "include/clock.h"

// TODO: Read XOSC section starting at pg.243 in rp2040 datasheet
void enable_clock() {
    uint32_t delay = 47;
    // 0. Setup XOSC delay

    // 1. Enable XOSC

    // 2. Wait till XOSC is ready

    // 3. Setup clk_ref so you can setup clock_sys

    // Lets your reference know you're running at 12MHZ
    PUT32(FC0_REF_KHZ, 0x12000);
}

//TODO: Use the timer section to make a delay_us function
void delay_us(uint32_t delay) {
    // 0. Read low half of timer
    uint64_t low = GET32(TIMER_READ_LOW);
    // 1. Read high half of timer
    uint64_t high = GET32(TIMER_READ_HIGH); 
    // 2. combine times into one uint64_t and then loop till the time is up
}

void delay_ms(uint32_t delay) {
    //TODO: call delay_us accordingly
}
