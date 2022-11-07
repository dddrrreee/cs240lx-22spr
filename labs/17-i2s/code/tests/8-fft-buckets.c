/*
 * Display the intensity of different frequency buckets 
 * on the neopixel ring. Each neopixel represents a "bucket"
 * (a group of neighboring frequencies) and its brightness 
 * represents the current intensity of that bucket.
 */

#include "rpi.h"
#include "fft.h"
#include "i2s.h"
#include "neopixel.h"

#define LOG2_FFT_LEN 10
#define FFT_LEN (1 << LOG2_FFT_LEN)

#define FS 44100

#define NEOPIX_PIN 2
#define NEOPIX_LEN 16
#define NEOPIX_MIN_FREQ 40
#define NEOPIX_MAX_FREQ 4000
#define NEOPIX_DIV 32

#define START_IDX ((NEOPIX_MIN_FREQ * FFT_LEN)/ FS)
#define FREQS_PER_BUCKET ((NEOPIX_MAX_FREQ - NEOPIX_MIN_FREQ) / (NEOPIX_LEN) / (FS / FFT_LEN))

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

        for (int i = 0; i < NEOPIX_LEN; i++) {
            // for each bucket (1 per neopixel...)
            int32_t acc = 0;
            for (int j = 0; j < FREQS_PER_BUCKET; j++) {
                // accumulate the magnitude squared of all the frequencies in this bucket
                int fft_idx = START_IDX + (FREQS_PER_BUCKET*i) + j;
                acc += fft_fixed_mul_q15(real[i], real[i]) + fft_fixed_mul_q15(imag[i], imag[i]);
            }
            // clip at 0xFF
            int val = (acc > 0xFF * NEOPIX_DIV) ? 0xFF : acc / NEOPIX_DIV;
            neopix_write(neo, i, val, val, val); // white
        }
        neopix_flush(neo);
    }

    clean_reboot();

}