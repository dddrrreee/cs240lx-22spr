// example for how to measure the cost of different operations.
// to make it easier to see what is going on, you might want to 
// pull pieces of code out into a routine.   you may need to add
// it to a different file to stop inlining.
#include "rpi.h"
#include "cycle-count.h"
#include "measure.h"

static volatile int x;

void measure(const char *msg) {
    printk("\n%s: measuring cost of different operations\n", msg);
    TIME_CYC_PRINT("\tempty measurement: ", gcc_mb());
    TIME_CYC_PRINT("\tNULL deref: ", *(volatile unsigned *)0);

    // interesting: flip these around!  "what is going on?"
    TIME_CYC_PRINT("\tassign: ", x = 1);
    TIME_CYC_PRINT("\tPUT32: ", put32(&x,1));

    // add some other interesting measurments or break them out
    // to see what is going on.

    TIME_CYC_PRINT("\tread/write barrier", dev_barrier());
    TIME_CYC_PRINT("\tread barrier", dmb());
    TIME_CYC_PRINT("\tsafe timer", timer_get_usec());
    TIME_CYC_PRINT("\tunsafe timer", timer_get_usec_raw());
    // macro expansion messes up the printk
    printk("\t<cycle_cnt_read()>: %d\n", TIME_CYC(cycle_cnt_read()));
    printk("-----------------------------------------\n");
}

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

    // play with this and do a bunch of measurements: see if they fluctuate
    measure("uncached trial 1");
    measure("uncached trial 2");

    // turn on icache
    enable_cache();

    // note, this is different from above!  "why?"
    printk("these will be different: from the previous: why?\n");
    start = cycle_cnt_read();
    end = cycle_cnt_read();
    printk("time to do a cached empty measurement: %d\n", end - start);

    // do a bunch of measurements: see if they fluctuate
    measure("cached trial 1");
    measure("cached trial 2");

    // measure using callouts to stop gcc from inlining.
    printk("why are these different?\n");
    printk("time to call out to assign: %d\n", measure_assign(1));
    printk("time to call out to assign: %d\n", measure_assign(1));
    printk("time to call out to assign: %d\n", measure_assign(1));
    printk("time to call out to put32: %d\n", measure_PUT32(1));
    printk("time to call out to put32: %d\n", measure_PUT32(1));
    printk("time to call out to put32: %d\n", measure_PUT32(1));
    printk("time to do empty measurement using callout: %d\n", measure_empty());


    clean_reboot();
}
