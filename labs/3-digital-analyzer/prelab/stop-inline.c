// if you need to prevent inlining put your routines here.

#include "rpi.h"
#include "cycle-count.h"
#include "measure.h"

static int volatile v;

unsigned measure_NULL(void) {
    unsigned s = cycle_cnt_read();
    gcc_mb();
    *(volatile unsigned *)0;
    gcc_mb();
    unsigned e = cycle_cnt_read();
    return e - s;
}

unsigned measure_assign(int x) {
    unsigned s = cycle_cnt_read();
    v = 1;
    unsigned e = cycle_cnt_read();
    return e - s;
}

unsigned measure_PUT32(int x) {
    unsigned s = cycle_cnt_read();
    put32(&v, x);
    unsigned e = cycle_cnt_read();
    return e - s;
}

unsigned measure_empty(void) {
    unsigned s = cycle_cnt_read();
    unsigned e = cycle_cnt_read();
    return e - s;
}

unsigned measure_nop1(void) {
    unsigned s = cycle_cnt_read();
    asm volatile ("nop");
    unsigned e = cycle_cnt_read();
    return e - s;
}

unsigned measure_nop2(void) {
    unsigned s = cycle_cnt_read();
    asm volatile ("nop");
    asm volatile ("nop");
    unsigned e = cycle_cnt_read();
    return e - s;
}

unsigned measure_nop3(void) {
    unsigned s = cycle_cnt_read();
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    unsigned e = cycle_cnt_read();
    return e - s;
}
unsigned measure_nop4(void) {
    unsigned s = cycle_cnt_read();
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    unsigned e = cycle_cnt_read();
    return e - s;
}
