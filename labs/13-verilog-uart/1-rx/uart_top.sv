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
    // TODO: localparam div = ...;

    counter #(.M(div)) baud_generator (
        .clk, .rst(rst),
        .max_tick(tick)
    );

    logic [7:0] rx_data;

    uart_rx rx_unit (
        .clk, .rst(rst),
        .rx(rx), .tick(tick),
        .rx_done_tick(), .dout(rx_data)
    );

    assign led_r = rx_data == 8'd114; // lowercase 'r'
    assign led_g = rx_data == 8'd103; // lowercase 'g'
    assign led_b = rx_data == 8'd98;  // lowercase 'b'
endmodule
