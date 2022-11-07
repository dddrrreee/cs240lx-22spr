module kernel.arch.riscv32.timer;

import csr = kernel.arch.riscv.csr;
import sys = kernel.sys;

void init() {
}

uint cycles() {
    return csr.read!(csr.Reg.mcycle)();
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
