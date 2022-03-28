#include "rpi.h"

static int in_code_segment(uint32_t addr) {
    extern char __code_start__, __code_end__;

    return addr >= (uint32_t)&__code_start__
        && addr < (uint32_t)&__code_end__;
}
