module kernel.board.raspi.uart;

import cpu = kernel.cpu;
import bits = ulib.bits;
import mmio = kernel.mmio;
import device = kernel.board.raspi.device;
import gpio = kernel.board.raspi.gpio;
import sys = kernel.board.raspi.sys;

struct AuxPeriphs {
    uint io;
    uint ier;
    uint iir;
    uint lcr;
    uint mcr;
    uint lsr;
    uint msr;
    uint scratch;
    uint cntl;
    uint stat;
    uint baud;
}

enum enable_uart = 1;
enum rx_enable = 1 << 0;
enum tx_enable = 1 << 1;
enum clear_tx_fifo = 1 << 1;
enum clear_rx_fifo = 1 << 2;
enum clear_fifos = clear_tx_fifo | clear_rx_fifo;
enum iir_reset = (0b11 << 6) | 1;

enum aux_enables = cast(uint*)(device.base + 0x215004);
enum uart = cast(AuxPeriphs*)(device.base + 0x215040);

void init(uint baud) {
    gpio.set_func(gpio.PinType.tx, gpio.FuncType.alt5);
    gpio.set_func(gpio.PinType.rx, gpio.FuncType.alt5);

    cpu.dsb();

    mmio.st(aux_enables, mmio.ld(aux_enables) | enable_uart);

    cpu.dsb();

    mmio.st(&uart.cntl, 0);
    mmio.st(&uart.ier, 0);
    mmio.st(&uart.lcr, 0b11);
    mmio.st(&uart.mcr, 0);
    mmio.st(&uart.iir, iir_reset | clear_fifos);
    mmio.st(&uart.baud, sys.gpu_freq / (baud * 8) - 1);
    mmio.st(&uart.cntl, rx_enable | tx_enable);

    cpu.dsb();
}

bool rx_empty() {
    return bits.get(mmio.ld(&uart.stat), 0) == 0;
}

uint rx_sz() {
    return bits.get(mmio.ld(&uart.stat), 19, 16);
}

bool can_tx() {
    return bits.get(mmio.ld(&uart.stat), 1) != 0;
}

ubyte rx() {
    cpu.dsb();
    while (rx_empty()) {
    }
    ubyte c = mmio.ld(&uart.io) & 0xff;
    cpu.dsb();
    return c;
}

void tx(ubyte c) {
    cpu.dsb();
    while (!can_tx()) {
    }
    mmio.st(&uart.io, c & 0xff);
    cpu.dsb();
}

bool tx_empty() {
    cpu.dsb();
    return bits.get(mmio.ld(&uart.stat), 9) == 1;
}

void tx_flush() {
    while (!tx_empty()) {
    }
}
