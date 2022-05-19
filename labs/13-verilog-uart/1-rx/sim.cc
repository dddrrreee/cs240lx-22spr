#include <stdlib.h>
#include "Vuart_top_uart_rx.h"
#include "Vuart_top_uart_top.h"
#include "Vuart_top.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define DEBUG 0

#if (DEBUG == 1)
#define dprintf(args...) printf(args)
#else
#define dprintf(args...)
#endif

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

static int simtime = 0;

static int tx(Vuart_top* dut, VerilatedVcdC* tfp, int cycles_per_bit, int bit) {
    int saw_done = 0;
    dut->rx = bit;
    for (int i = 0; i < cycles_per_bit; i++) {
        dut->clk = 0;
        dut->eval();
        tfp->dump(simtime++);
        dut->clk = 1;
        dut->eval();
        tfp->dump(simtime++);

        if (dut->uart_top->rx_unit->rx_done_tick) {
            dprintf("%d: saw rx_done_tick\n", simtime);
            saw_done = 1;
        }
    }
    return saw_done;
}

static void send_byte(Vuart_top* dut, VerilatedVcdC* tfp, int cycles_per_bit, uint8_t byte) {
    // start bit
    dprintf("%d: start bit\n", simtime);
    tx(dut, tfp, cycles_per_bit, 0);

    // data
    uint8_t tx_byte = byte;
    printf("%d: transmitting %d...\n", simtime, tx_byte);
    for (int i = 0; i < 8; i++) {
        dprintf("%d: data bit %d\n", simtime, tx_byte & 0x1);
        tx(dut, tfp, cycles_per_bit, tx_byte & 0x1);
        dprintf("%d: dout: 0b" BYTE_TO_BINARY_PATTERN "\n", simtime, BYTE_TO_BINARY(dut->uart_top->rx_unit->dout));
        tx_byte >>= 1;
    }

    // stop bit
    dprintf("%d: stop bit\n", simtime);
    int done = tx(dut, tfp, cycles_per_bit, 1);

    printf("%d: received %d\n", simtime, dut->uart_top->rx_unit->dout);

    // check
    if (dut->uart_top->rx_unit->dout == byte && done) {
        printf("PASS\n");
    } else {
        if (dut->uart_top->rx_unit->dout != byte) {
            printf("FAIL: data mistmatch: %d != %d\n", dut->uart_top->rx_unit->dout, byte);
        }
        if (!done) {
            printf("FAIL: did not see rx_done_tick\n");
        }
    }
}

#define CLK_MHZ 48

int main() {
    int cycles_per_bit = CLK_MHZ * 1000000 / 19200;

    Verilated::traceEverOn(true);

    Vuart_top* dut = new Vuart_top;

    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("trace.vcd");
    int time = 0;
    int clk = 0;

    // idle
    dprintf("%d: idle\n", simtime);
    tx(dut, tfp, cycles_per_bit, 1);

    for (int i = 0; i < 4; i++) {
        send_byte(dut, tfp, cycles_per_bit, rand());
    }

    tfp->close();

    return 0;
}

