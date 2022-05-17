#include <stdlib.h>
#include "Vmux.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main() {
    Verilated::traceEverOn(true);

    Vmux* dut = new Vmux;

    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("trace.vcd");
    int time = 0;

    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 2; b++) {
            for (int sel = 0; sel < 2; sel++) {
                dut->a = a;
                dut->b = b;
                dut->sel = sel;
                dut->eval();

                printf("a=%d, b=%d, sel=%d; out=%d\n", a, b, sel, dut->out);
                tfp->dump(time++);
            }
        }
    }

    tfp->close();
    return 0;
}

