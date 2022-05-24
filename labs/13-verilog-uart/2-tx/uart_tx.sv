module uart_tx
    (
        input logic clk, rst,
        input logic tx_start,      // pulse one tick when transmission should begin
        input logic tick,          // baud rate oversampled tick
        input logic [7:0] din,     // data to send (user must keep data valid
                                   // at least until tx_done_tick is asserted)
        output logic tx_done_tick, // pulse one tick when done
        output logic tx            // serial data
    );

    /* verilator public_module */

    typedef enum {idle, start, data, stop} state_t;
    state_t state_reg = idle;
    state_t state_next;

    // TODO
endmodule
