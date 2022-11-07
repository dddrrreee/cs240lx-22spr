module kernel.mmio;

import core.bitop;

void st(uint* ptr, uint value) {
    volatileStore(ptr, value);
}

uint ld(uint* ptr) {
    return volatileLoad(ptr);
}
