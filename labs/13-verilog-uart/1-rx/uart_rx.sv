module uart_rx
    (
        input logic clk, rst,
        input logic rx,            // serial data
        input logic tick,          // baud rate oversampled tick
        output logic rx_done_tick, // pulse one tick when done
        output logic [7:0] dout    // output data
    );

    /* verilator public_module */

    parameter idle = 2'b00;
    parameter start = 2'b01;
    parameter data = 2'b10;
    parameter stop = 2'b11;

    // typedef enum {idle, start, data, stop} state_t;
    reg[1:0] state_reg = idle;
    reg[1:0] state_next;

    assign dout = 1;
    // TODO
endmodule
