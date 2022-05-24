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

static std::vector<uint8_t> rxed_from_dut;

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
        rxed_from_dut.push_back(dut->tx);

        if (dut->uart_top->rx_unit->rx_done_tick) {
            saw_done = 1;
        }
    }
    return saw_done;
}

static void send_byte(Vuart_top* dut, VerilatedVcdC* tfp, int cycles_per_bit, uint8_t byte) {
    // start bit
    tx(dut, tfp, cycles_per_bit, 0);

    // data
    uint8_t tx_byte = byte;
    printf("%d: transmitting %d...\n", simtime, tx_byte);
    for (int i = 0; i < 8; i++) {
        tx(dut, tfp, cycles_per_bit, tx_byte & 0x1);
        tx_byte >>= 1;
    }

    // stop bit
    int done = tx(dut, tfp, cycles_per_bit, 1);

    // check
    if (dut->uart_top->rx_unit->dout == byte && done) {
        dprintf("PASS (rx): received %d\n", dut->uart_top->rx_unit->dout);
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
    tx(dut, tfp, cycles_per_bit, 1);

    std::vector<uint8_t> sent_bytes;

    for (int i = 0; i < 4; i++) {
        uint8_t to_send = rand();
        sent_bytes.push_back(to_send);
        send_byte(dut, tfp, cycles_per_bit, to_send);
    }

    // send an additional zero byte to give enough simulation time for the dut
    // to transmit back the bytes we sent earlier.
    send_byte(dut, tfp, cycles_per_bit, 0);

    dprintf("Cycles per bit: %d\n", cycles_per_bit);
    printf("Checking tx...\n");

    int tx_clk = 0;

    for (uint8_t expected : sent_bytes) {
        if (rxed_from_dut[tx_clk] != 1) {
            printf("FAIL: first bit transmitted is not 0\n");
            return 1;
        }
        tx_clk++;
        for (int i = 1; i < rxed_from_dut.size(); i++, tx_clk++) {
            if (rxed_from_dut[tx_clk] == 0) {
                dprintf("%d: saw start of start bit\n", tx_clk);
                break;
            }
        }
        // start bit
        for (int i = 0; i < cycles_per_bit / 2; i++, tx_clk++) {
            if (rxed_from_dut[tx_clk] != 0) {
                printf("%d: FAIL: start bit is not 0\n", tx_clk);
                return 1;
            }
        }

        uint8_t byte_from_dut = 0;
        for (int i = 0; i < 8; i++) {
            tx_clk += cycles_per_bit;
            byte_from_dut |= (rxed_from_dut[tx_clk] << i);
        }
        tx_clk += cycles_per_bit;
        if (rxed_from_dut[tx_clk] != 1) {
            printf("%d: FAIL: stop bit is not 1\n", tx_clk);
            return 1;
        }

        if (byte_from_dut != expected) {
            printf("FAIL: FPGA echoed %d, expected %d\n", byte_from_dut, expected);
        } else {
            printf("PASS: FPGA echoed %d\n", byte_from_dut);
        }
    }

    tfp->close();

    return 0;
}

