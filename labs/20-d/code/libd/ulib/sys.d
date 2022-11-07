module ulib.sys;

import io = ulib.io;
import sys = kernel.sys;
import uart = kernel.uart;

__gshared io.File stdout = io.File(function(ubyte c) { uart.tx(c); });

void exit(ubyte code) {
    sys.reboot();
}
