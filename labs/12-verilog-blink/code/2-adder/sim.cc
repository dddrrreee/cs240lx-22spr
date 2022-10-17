#include <stdlib.h>
#include "Vadder.h"
#include "verilated.h"

void check(Vadder* dut, uint32_t a, uint32_t b) {
    dut->a = a;
    dut->b = b;
    dut->eval();
    printf("check: %x + %x = %x\n", a, b, dut->sum);
    printf("check: %u + %u = %u\n", a, b, dut->sum);
    if(dut->sum != a + b) 
        printf("failed!\n");
    assert(dut->sum == a + b);
}

int main() {
    Vadder* dut = new Vadder;

    printf("checking trivial 0 + 0\n");
    check(dut, 0, 0);
    printf("checking trivial 1 + 0\n");
    check(dut, 1, 0);
    printf("checking trivial 0 + 1\n");
    check(dut, 0, 1);
    printf("checking trivial 1 + 1\n");
    check(dut, 1, 1);

    printf("checking random: no wrap around\n");
    for (int i = 0; i < 10; i++) {
        uint32_t a = rand() % 0xffff;
        uint32_t b = rand() % 0xffff;
        check(dut, a, b);
    }

    printf("checking random\n");

    for (int i = 0; i < 10; i++) {
        uint32_t a = rand();
        uint32_t b = rand();
        check(dut, a, b);
    }

    check(dut, (uint32_t) -1, 1);
    check(dut, (uint32_t) -1, (uint32_t) -1);

    return 0;
}

