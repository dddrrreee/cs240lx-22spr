module kernel.board.raspi.sys;

import uart = kernel.uart;
import mmio = kernel.mmio;
import sys = kernel.cpu;
import device = kernel.board.raspi.device;

version (raspi1ap) {
    enum gpu_freq = 250 * 1000 * 1000;
    enum core_freq = 700 * 1000 * 1000;
}
version (raspi3b) {
    enum gpu_freq = 250 * 1000 * 1000;
    enum core_freq = 700 * 1000 * 1000;
}
version (raspi4b) {
    enum gpu_freq = 250 * 1000 * 1000;
    enum core_freq = 700 * 1000 * 1000;
}

void reboot() {
    // for the first part of the lab we don't have io.writeln yet, so just use
    // uart.tx directly
    enum done = "DONE!!!\n";
    foreach (c; done) {
        uart.tx(c);
    }
    uart.tx_flush();
    sys.dsb();
    uint* pm_rstc = cast(uint*)(device.base + 0x10001c);
    uint* pm_wdog = cast(uint*)(device.base + 0x100024);

    const pm_password = 0x5a000000;
    const pm_rstc_wrcfg_full_reset = 0x20;

    mmio.st(pm_wdog, pm_password | 1);
    mmio.st(pm_rstc, pm_password | pm_rstc_wrcfg_full_reset);
    while (true) {
    }
}
