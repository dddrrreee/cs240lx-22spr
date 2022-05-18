#include <stdlib.h>
#include "Vuart_rx.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int simtime = 0;

void tx(Vuart_rx* dut, VerilatedVcdC* tfp, unsigned bit) {
    int clk = 0;
    int tick = 0;
    dut->rx = bit;
    for (int i = 0; i < 32; i++) {
        dut->clk = clk;
        dut->tick = tick;
        if (i % 2 == 0) {
            tick = !tick;
        }
        clk = !clk;
        dut->eval();
        tfp->dump(simtime++);
    }
}

int main() {
    Verilated::traceEverOn(true);

    Vuart_rx* dut = new Vuart_rx;

    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("trace.vcd");
    int clk = 0;
    int tick = 0;
    dut->rx = 1;

    tx(dut, tfp, 1);
    tx(dut, tfp, 0);

    for (int i = 0; i < 8; i++) {
        tx(dut, tfp, i % 2);
    }

    tx(dut, tfp, 1);

    tfp->close();

    return 0;
}
