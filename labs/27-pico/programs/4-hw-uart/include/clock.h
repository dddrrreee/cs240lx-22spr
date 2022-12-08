#ifndef __CLOCK_H__
#define __CLOCK_H__
#include <stdint.h>

#define CLK_BASE 0x40008000
// need to write 0x2
#define CLK_REF_CTRL CLK_BASE + 0x30 
// need to write 0x0 switch to ref
#define CLK_SYS_CTRL CLK_BASE + 0x3c 

#define MAX_CYCLES 0x00FFFFFF


#define STK_CSR 0xE000E010
#define STK_RVR 0xE000E014
#define STK_CVR 0xE000E018

// Check page 245 in the rp2040 datasheet for more info
#define XOSC_BASE 0x40024000
enum {
    XOSC_CTRL = XOSC_BASE + 0x00,
    XOSC_STATUS = XOSC_BASE + 0x04,
    XOSC_DORMANT = XOSC_BASE + 0x08,
    XOSC_STARTUP = XOSC_BASE + 0x0c,
    XOSC_COUNT = XOSC_BASE + 0x1c,
    FC0_REF_KHZ = XOSC_BASE + 0x80,
    FC0_INTERVAL = XOSC_BASE + 0x90,
    FC0_SRC = XOSC_BASE + 0x94,
    FC0_STATUS = XOSC_BASE + 0x98
};

#define TIMER_BASE 0x40054000

#define CLK_PERI_DEFAULT_SPEED 0xB71B00

enum {
    CLK_PERI_CTRL = CLK_BASE + 0x48
};

// Always read low before high
enum {
    TIMER_READ_HIGH = TIMER_BASE + 0x08,
    TIMER_READ_LOW = TIMER_BASE + 0x0c
};

void enable_clock();
void delay_us(uint32_t delay);
void delay_ms(uint32_t delay);

#endif