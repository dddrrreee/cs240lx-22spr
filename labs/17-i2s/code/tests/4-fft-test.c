/*
 * Test that the FFT is working as expected
 */

#include "rpi.h"
#include "fft.h"

#define LOG2_N 4
#define N (1 << LOG2_N)

void notmain(void) {
    enable_cache();

    // alternating samples: should only see one nonzero frequency at midpoint
    int16_t real[N] = {1024,-1024,1024,-1024,1024,-1024,1024,-1024,1024,-1024,1024,-1024,1024,-1024,1024,-1024};
    int16_t imag[N] = {0};
    printk("IN\n");
    for (int i = 0; i < N; i++) {
        printk("%d: %d + %dj\n", i, real[i], imag[i]);
    }
    
    printk("\nFWD\n");
    fft_fixed_cfft(real, imag, LOG2_N, 0);

    for (int i = 0; i < N; i++) {
        printk("%d: %d + %dj\n", i, real[i], imag[i]);
    }

    printk("\nINV\n");
    fft_fixed_cfft(real, imag, LOG2_N, 1);


    for (int i = 0; i < N; i++) {
        printk("%d: %d + %dj\n", i, real[i], imag[i]);
    }
    printk("\n");

    clean_reboot();
}