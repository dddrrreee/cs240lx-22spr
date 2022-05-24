#include <stdlib.h>
#include "Vled_top.h"
#include "verilated.h"

int main() {
    Vled_top* dut = new Vled_top;

    dut->eval();
    printf("led=%d\n", dut->led_r);

    return 0;
}

