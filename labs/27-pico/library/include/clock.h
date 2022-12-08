#ifndef __CLOCK_H__
#define __CLOCK_H__
#include <stdint.h>

#define CLK_BASE 0x40008000
// need to write 0x2
#define CLK_REF_CTRL CLK_BASE + 0x30 
// need to write 0x0 switch to ref
#define CLK_SYS_CTRL CLK_BASE + 0x3c 

#define MAX_CYCLES 0x00FFFFFF

#define XOSC_BASE 0x40024000

#define STK_CSR 0xE000E010
#define STK_RVR 0xE000E014
#define STK_CVR 0xE000E018

enum {
    XOSC_CTRL = XOSC_BASE + 0x00,
    XOSC_STATUS = XOSC_BASE + 0x04,
    XOSC_DORMANT = XOSC_BASE + 0x08,
    XOSC_STARTUP = XOSC_BASE + 0x0c,
    XOSC_COUNT = XOSC_BASE + 0x1c
};

void enable_clock();

void delay_us(uint32_t us);
void micro_seconds_start();
uint32_t micro_second_end();
void seconds_start();
#endif