/*
 * simple test to see that i2s is working
 */
#include "rpi.h"
#include "i2s.h"


void notmain(void) {
    enable_cache(); 

    i2s_init();

    // print i2s samples for a very long time
    // not timing accurate since printk is very slow
    // just use for a sanity check (power cycle the pi to stop it)
    for (int i = 0; i < 100000; i++) {
        int32_t sample = i2s_read_sample();
        printk("%x %d\n", sample, sample);  
    }

    output("done!\n");
}
