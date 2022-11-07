module kernel;

import uart = kernel.uart;
import timer = kernel.timer;
import sys = kernel.sys;

import io = ulib.io;
static import ulib.sys;

void init() {
    uart.init(115200);
    timer.init();
}
