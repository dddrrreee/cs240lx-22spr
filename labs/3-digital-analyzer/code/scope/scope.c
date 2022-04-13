#include "rpi.h"

// cycle counter routines.
#include "cycle-count.h"


// this defines the period: makes it easy to keep track and share
// with the test generator.
#include "../test-gen/test-gen.h"

// trivial logging code to compute the error given a known
// period.
#include "samples.h"

// derive this experimentally: check that your pi is the same!!
#define CYCLE_PER_SEC (700*1000*1000)

// some utility routines: you don't need to use them.
#include "cycle.h"

// implement this code and tune it.
unsigned 
scope(unsigned pin, log_ent_t *l, unsigned n_max, unsigned max_cycles) {
    unsigned v1, v0 = gpio_read(pin);

    // spin until the pin changes.
    while((v1 = gpio_read(pin)) == v0)
        ;


    // when we started sampling 
    unsigned start = cycle_cnt_read(), t = start;

    // sample until record max samples or until exceed <max_cycles>
    unsigned n = 0;
    for(; n < n_max;) {


        // write this code first: record sample when the pin
        // changes.  then start tuning the whole routine.
        unimplemented();


        // exit when we have run too long.
        if((cycle_cnt_read() - start) > max_cycles)  {
            printk("timeout! start=%d, t=%d, minux=%d\n",
                                 start,t,t-start);
            return n;
        }
    }
    return n;
}

void notmain(void) {
    // setup input pin.
    int pin = 21;
    gpio_set_input(pin);

    // make sure to init cycle counter hw.
    cycle_cnt_init();

#   define MAXSAMPLES 32
    log_ent_t log[MAXSAMPLES];

    // just to illustrate.  remove this.
    sample_ex(log, 10, CYCLE_PER_FLIP);

    // run 4 times before rebooting: makes things easier.
    // you can get rid of this.
    for(int i = 0; i < 4; i++) {
        unsigned n = scope(pin, log, MAXSAMPLES, sec_to_cycle(1));
        dump_samples(log, n, CYCLE_PER_FLIP);
    }
    clean_reboot();
}
