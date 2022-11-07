module kernel.arch.arm.timer;

import sys = kernel.sys;

version (LDC) {
    import ldc.llvmasm;

    void init() {
        __asm (
            "mcr p15, 0, $0, c15, c12, 0", "r", 1
        );
    }

    uint cycles() {
        uint cyc;
        __asm (
            "mrc p15, 0, $0, c15, c12, 1", "=*r", &cyc
        );
        return cyc;
    }
} else {
    void init() {
        asm {
            "mcr p15, 0, %0, c15, c12, 0" :  : "r"(1);
        }
    }

    uint cycles() {
        uint cyc;
        asm {
            "mrc p15, 0, %0, c15, c12, 1" : "=r"(cyc);
        }
        return cyc;
    }
}

void delay_us(uint us) {
    uint rb = cycles();
    while (1) {
        uint ra = cycles();
        if ((ra - rb) >= us * (sys.core_freq / (1000 * 1000))) {
            break;
        }
    }
}
