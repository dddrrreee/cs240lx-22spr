import math

FFT_SINE_LUT_SIZE = 1024
LOG2_FFT_SINE_LUT_SIZE = int(math.log2(FFT_SINE_LUT_SIZE))

ENTRIES_PER_LINE = 8

# fstrings are funky with these
open_brace = '{'
close_brace = '}'

header = \
f"""#ifndef FFT_SINE_LUT_H
#define FFT_SINE_LUT_H

#include "rpi.h"

#define FFT_SINE_LUT_SIZE {FFT_SINE_LUT_SIZE}
#define LOG2_FFT_SINE_LUT_SIZE {LOG2_FFT_SINE_LUT_SIZE}

// save space by using 3/4 of wave
int16_t FFT_SINE_LUT[FFT_SINE_LUT_SIZE - FFT_SINE_LUT_SIZE/4] = {open_brace}
"""

footer = """};

#endif // FFT_SINE_LUT_H
"""

with open("../fft_sine_lut.h", 'w') as f:
    f.write(header)

    for i in range(int(FFT_SINE_LUT_SIZE - FFT_SINE_LUT_SIZE/4)):
        if i % ENTRIES_PER_LINE == 0:
            f.write("   ")
        
        raw = math.sin(2 * math.pi * i / FFT_SINE_LUT_SIZE)
        fixed = int(raw * (2**15 - 1))

        f.write(f"{fixed:7d},")

        if i % ENTRIES_PER_LINE == ENTRIES_PER_LINE - 1:
            f.write("\n")

    f.write(footer)