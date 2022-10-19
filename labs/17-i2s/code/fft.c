#include "fft.h"
#include "fft_sine_lut.h"

// fixed point FFT implementation based on
// fix_fft by Tom Roberts 
// attempted to cleanup the code a bit since
// it was horrific C code from the late 80s

// fft_fixed_cfft() - perform forward/inverse complex FFT in-place
int32_t fft_fixed_cfft(int16_t *real, int16_t *imag, int16_t log2_len, unsigned inverse) {

    unsigned n = 1 << log2_len;

    if (n > FFT_SINE_LUT_SIZE) {
        panic("fft_fixed_cfft: FFT too long");
    }

    int32_t scale = 0;
    int32_t swap_idx = 0;

    // decimation in time - reorder data
    for (unsigned curr_idx = 1; curr_idx < n; curr_idx++) {
        unsigned len = n;
        do {
            len >>= 1;
        } while (swap_idx + len > n - 1);

        swap_idx = (swap_idx & (len - 1)) + len;
        if (swap_idx <= curr_idx) {
            continue;
        }

        int16_t temp_real = real[curr_idx];
        real[curr_idx] = real[swap_idx];
        real[swap_idx] = temp_real;
        int16_t temp_imag = imag[curr_idx];
        imag[curr_idx] = imag[swap_idx];
        imag[swap_idx] = temp_imag;
    }

    unsigned len = 1;
    int16_t log2_j_stride = LOG2_FFT_SINE_LUT_SIZE - 1;
    while (len < n) {
        int32_t shift = 1;
        if (inverse) {
            // determine if scaling is needed
            shift = 0;
            for (unsigned i = 0; i < n; i++) {
                int16_t j = real[i];
                if (j < 0)
                    j = -j;
                log2_len = imag[i];
                if (log2_len < 0)
                    log2_len = -log2_len;
                if (j > INT16_MAX || log2_len > INT16_MAX) {
                    shift = 1;
                    break;
                }
            }
            if (shift) {
                scale++;
            }
        }
        
        // perform butterfly operation
        unsigned i_stride = len * 2;
        for (unsigned curr = 0; curr < len; curr++) {
            int32_t j = curr << log2_j_stride;
            // compute "twiddle" from sine lookup table
            int16_t twid_real =  FFT_SINE_LUT[j+FFT_SINE_LUT_SIZE/4];
            int16_t twid_imag = -FFT_SINE_LUT[j];

            // invert imaginary part for ifft
            if (inverse) {
                twid_imag = -twid_imag;
            }

            if (shift) {
                twid_real /= 2;
                twid_imag /= 2;
            }
            for (unsigned i = curr; i < n; i += i_stride) {
                j = i + len;
                int16_t base_real = fft_fixed_mul_q15(twid_real, real[j]) - fft_fixed_mul_q15(twid_imag, imag[j]);
                int16_t base_imag = fft_fixed_mul_q15(twid_real, imag[j]) + fft_fixed_mul_q15(twid_imag, real[j]);
                int16_t curr_real = real[i];
                int16_t curr_imag = imag[i];
                if (shift) {
                    curr_real /= 2;
                    curr_imag /= 2;
                }
                real[j] = curr_real - base_real;
                imag[j] = curr_imag - base_imag;
                real[i] = curr_real + base_real;
                imag[i] = curr_imag + base_imag;
            }
        }
        log2_j_stride--;
        len = i_stride;
    }
    return scale;
}

// perform forward/inverse real FFT in-place
int32_t fft_fixed_rfft(int16_t *data, int32_t log2_len, unsigned inverse) {
    panic("fft_fixed_rfft: seems broken");
    
    int32_t i;
    unsigned N = 1 << (log2_len - 1);
    int32_t scale = 0;
    int16_t *real = data;
    int16_t *imag = &data[N];

    if (inverse) {
        scale = fft_fixed_cfft(imag, real, log2_len - 1, inverse);
    }
    for (i = 1; i < N; i += 2) {
        int16_t temp = data[N+i-1];
        data[N+i-1] = data[i];
        data[i] = temp;
    }
    if (!inverse) {
        scale = fft_fixed_cfft(imag, real, log2_len - 1, inverse);
    }
    return scale;
}
