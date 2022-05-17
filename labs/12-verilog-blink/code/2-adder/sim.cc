#include <stdlib.h>
#include "Vadder.h"
#include "verilated.h"

void check(Vadder* dut, uint32_t a, uint32_t b) {
    dut->a = a;
    dut->b = b;
    dut->eval();
    printf("check: %d + %d = %d\n", a, b, dut->sum);
    assert(dut->sum == a + b);
}

int main() {
    Vadder* dut = new Vadder;

    for (int i = 0; i < 10; i++) {
        uint32_t a = rand();
        uint32_t b = rand();
        check(dut, a, b);
    }

    check(dut, 0, 0);
    check(dut, (uint32_t) -1, 1);
    check(dut, (uint32_t) -1, (uint32_t) -1);

    return 0;
}

