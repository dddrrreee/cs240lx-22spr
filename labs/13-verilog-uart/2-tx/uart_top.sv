module uart_top
    (
        input logic clk,
        input logic rst,

        output logic led_r,
        output logic led_g,
        output logic led_b,

        input logic rx,
        output logic tx
    );

    logic tick;

    // baud rate of 19200 oversampled 16x on our CLK_MHZ clock:
    // TODO: (copy from part 1)
    // localparam div = ...;

    counter #(.M(div)) baud_generator (
        .clk, .rst(rst),
        .max_tick(tick)
    );

    logic [7:0] rx_data;
    logic rx_done_tick;

    uart_rx rx_unit (
        .clk, .rst(rst),
        .rx(rx), .tick,
        .rx_done_tick, .dout(rx_data)
    );

    logic full;
    logic [7:0] rx_data_buf;
    logic tx_done_tick;

    flag_buf rx_interface (
        .clk, .rst(rst),
        .clr_flag(tx_done_tick), .set_flag(rx_done_tick),
        .din(rx_data),
        .flag(full),
        .dout(rx_data_buf)
    );

    uart_tx tx_unit (
        .clk, .rst(rst),
        .tx_start(full), .tick,
        .din(rx_data_buf), .tx_done_tick,
        .tx(tx)
    );

    assign led_r = rx_data == 8'd114; // lowercase 'r'
    assign led_g = rx_data == 8'd103; // lowercase 'g'
    assign led_b = rx_data == 8'd98;  // lowercase 'b'
endmodule
