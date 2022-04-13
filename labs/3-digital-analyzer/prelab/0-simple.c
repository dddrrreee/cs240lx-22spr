// simple example of how to measure using the raw cycle counter.
#include "rpi.h"
#include "cycle-count.h"
#include "measure.h"

static volatile int x;

// demonstrate that we can dereference NULL :)
unsigned measure_null(void) {
    unsigned start = cycle_cnt_read();
    *(volatile unsigned *)0;
    unsigned end = cycle_cnt_read();
    return end - start;
}

void notmain(void) {
    cycle_cnt_init();

    // example of how to use the counters raw
    unsigned start = cycle_cnt_read();
    unsigned end = cycle_cnt_read();
    printk("time to do an uncached measurement: %d\n", end - start);

    // put it in a function call so you can verify it's getting called.
    printk("time to dereference NULL: %d\n", measure_null());

    // measure using callouts to stop gcc from inlining.
    printk("time to assign using callout: %d\n", measure_assign(1));
    printk("time to assign using callout: %d\n", measure_assign(1));
    printk("time to assign using callout: %d\n", measure_assign(1));
    printk("time to put32 using callout: %d\n", measure_PUT32(1));
    printk("time to put32 using callout: %d\n", measure_PUT32(1));
    printk("time to put32 using callout: %d\n", measure_PUT32(1));
    printk("time to do empty measurement using callout: %d\n", measure_empty());
    printk("time to do nop1 measurement using callout: %d\n", measure_nop1());
    printk("time to do nop2 measurement using callout: %d\n", measure_nop2());
    printk("time to do nop3 measurement using callout: %d\n", measure_nop3());
    printk("time to do nop4 measurement using callout: %d\n", measure_nop4());

    clean_reboot();

}
