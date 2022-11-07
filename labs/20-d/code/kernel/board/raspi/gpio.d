module kernel.board.raspi.gpio;

// for mmio.st and mmio.ld
import mmio = kernel.mmio;
// for accessing the MMIO base: device.base
import device = kernel.board.raspi.device;

enum PinType {
    tx = 14,
    rx = 15,
    sda = 2,
    scl = 3,
}

enum FuncType {
    input = 0,
    output = 1,
    alt0 = 4,
    alt1 = 5,
    alt2 = 6,
    alt3 = 7,
    alt4 = 3,
    alt5 = 2,
}

enum base = 0x200000;

void set_func(uint pin, FuncType fn) {
    // TODO
}

void set_output(uint pin) {
    set_func(pin, FuncType.output);
}

void set_input(uint pin) {
    set_func(pin, FuncType.input);
}

void set_on(uint pin) {
    // TODO
}

void set_off(uint pin) {
    // TODO
}

bool read(uint pin) {
    // TODO
    return false;
}
