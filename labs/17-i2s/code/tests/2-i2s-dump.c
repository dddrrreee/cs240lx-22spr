/*
 * record i2s samples to a buffer then print them out
 * so you can generate a wav file
 * usage: make 2>&1 | grep DUMP | tr -d DUMP > ../py/dump.txt
 */
#include "rpi.h"
#include "i2s.h"

#define SECS 5
#define SAMPLE_RATE 44100
#define N (SAMPLE_RATE * SECS)

void notmain(void) {
    enable_cache(); 

    int32_t *buf = (int32_t *)kmalloc(N * sizeof(int32_t));

    i2s_init();

    unsigned start = timer_get_usec();
    for (int i = 0; i < N; i++) {
        buf[i] = i2s_read_sample();
    }
    unsigned end = timer_get_usec();

    for (int i = 0; i < N; i++) {
        printk("DUMP%x\n", buf[i]);  
    }

    printk("Measured %d samples in %d us\n", N, end - start);

    output("done!\n");
}
