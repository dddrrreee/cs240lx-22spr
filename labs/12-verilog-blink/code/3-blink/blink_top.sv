module blink_top
    (
        input logic clk,
        input logic rst,
        output logic led_r,
        output logic led_g,
        output logic led_b,
        input logic rx,
        output logic tx
    );

    // the led should toggle once per second
    localparam M = `CLK_MHZ * 1_000_000;

    logic [$clog2(M)-1:0] count;

    counter #(.M(M)) u_counter (
        .clk, .rst (rst), .q (count)
    );

    assign led_g = count[$clog2(M)-1];
endmodule
