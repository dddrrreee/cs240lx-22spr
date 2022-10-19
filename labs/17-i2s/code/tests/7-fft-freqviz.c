/*
 * Print out fundamental frequency and display
 * an increasing bar on the neopixel ring
 */

#include "rpi.h"
#include "fft.h"
#include "i2s.h"
#include "neopixel.h"

#define LOG2_FFT_LEN 10
#define FFT_LEN (1 << LOG2_FFT_LEN)

#define FS 44100
// attempt to reject harmonics. change this if you're
// seeing multiples of the fundamental frequency 
#define MAX_THRESH_FACTOR 5 / 4

#define NEOPIX_PIN 2
#define NEOPIX_LEN 16
#define NEOPIX_MIN_FREQ 430
#define NEOPIX_MAX_FREQ 800

int get_idx(int freq) {
    if (freq < NEOPIX_MIN_FREQ || freq > NEOPIX_MAX_FREQ) {
        return -1;
    }

    return ((freq - NEOPIX_MIN_FREQ) * NEOPIX_LEN) / (NEOPIX_MAX_FREQ - NEOPIX_MIN_FREQ);
}

void notmain(void) {
    enable_cache();
    i2s_init();
    neo_t neo = neopix_init(NEOPIX_PIN, NEOPIX_LEN);

    int16_t real[FFT_LEN] = {0};
    int16_t imag[FFT_LEN] = {0};

    while (1) {

        // real samples: set imaginary part to 0
        for (int i = 0; i < FFT_LEN; i++) {
            real[i] = to_q15(i2s_read_sample());
            imag[i] = 0;
        }

        fft_fixed_cfft(real, imag, LOG2_FFT_LEN, 0);

        int16_t data_max = 0;
        int16_t data_max_idx = 0;

        for (int i = 0; i < FFT_LEN; i++) {
            int32_t mag = fft_fixed_mul_q15(real[i], real[i]) + fft_fixed_mul_q15(imag[i], imag[i]);
            // attempt to reject harmonics by requiring higher frequencies to be some factor larger
            if (mag > data_max * MAX_THRESH_FACTOR) {
                data_max = mag;
                data_max_idx = i;
            }
        }

        int16_t freq = data_max_idx * FS / FFT_LEN;

        int neopix_idx = get_idx(freq);
        for (int i = 0; i < neopix_idx; i++) {
            neopix_write(neo, i, 0x80, 0x80, 0x80);
        }
        neopix_flush(neo);

        printk("%dHz, index %d\n", freq, neopix_idx);

    }

    clean_reboot();

}