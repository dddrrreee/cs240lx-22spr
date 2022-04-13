// example for how to measure using helper macros and cache/uncache.
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

void measure(const char *msg) {

    printk("-----------------------------------------------------\n");
    printk("measuring: <%s>\n", msg);

    // example of how to use the counters raw
    unsigned start = cycle_cnt_read();
    unsigned end = cycle_cnt_read();
    printk("\ttime to do an empty measurement: %d\n", end - start);
    TIME_CYC_PRINT("\tempty: ", {});
    TIME_CYC_PRINT("\tempty: ", {});
    TIME_CYC_PRINT("\tempty: ", {});

    // put it in a function call so you can verify it's getting called.
    printk("\ttime to dereference NULL: %d\n", measure_null());
    printk("\ttime to dereference NULL: %d\n", measure_null());
    TIME_CYC_PRINT("\tnull: ", *(volatile unsigned *)0);
    TIME_CYC_PRINT("\tnull: ", *(volatile unsigned *)0);

    TIME_CYC_PRINT("\tcallout null: ", measure_null());
    TIME_CYC_PRINT("\tcallout null: ", measure_null());

    // measure using callouts to stop gcc from inlining.
    printk("\ttime to assign using callout: %d\n", measure_assign(1));
    printk("\ttime to assign using callout: %d\n", measure_assign(1));
    TIME_CYC_PRINT("\tPUT32: ", x = 1);
    TIME_CYC_PRINT("\tPUT32: ", x = 1);

    printk("\ttime to put32 using callout: %d\n", measure_PUT32(1));
    printk("\ttime to put32 using callout: %d\n", measure_PUT32(1));
    TIME_CYC_PRINT("\tPUT32: ", put32(&x,1));
    TIME_CYC_PRINT("\tPUT32: ", put32(&x,1));

    printk("-----------------------------------------------------\n");
}


void notmain(void) {
    cycle_cnt_init();


    measure("uncached");
    enable_cache();
    measure("cached");

    clean_reboot();

}
