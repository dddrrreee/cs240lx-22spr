/*
 * See performance of the FFT
 */

#include "rpi.h"
#include "fft.h"

#define LOG2_N 8
#define N (1 << LOG2_N)

#define NUM_TRIALS 1024

void notmain(void) {

    int16_t real[N];
    int16_t imag[N];

    // N samples of 1024 + 0i
    for (int i = 0; i < N; i++) {
        real[i] = 1024;
        imag[i] = 0;
    }


    uint32_t start = timer_get_usec();
    for (int i = 0; i < NUM_TRIALS; i++) {
        fft_fixed_cfft(real, imag, LOG2_N, 0);
    }
    uint32_t end = timer_get_usec();

    printk("%dus per %d-point FFT\n", (end - start) / NUM_TRIALS, N);

    clean_reboot();
}