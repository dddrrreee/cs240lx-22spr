module led_top
    (
        input logic clk,
        input logic rst,

        output logic led_r,
        output logic led_g,
        output logic led_b,

        input logic rx,
        output logic tx
    );
    // TODO
    assign led_r = 1;
    //assign led_g = 1;
    // assign led_b = clk & 1;
endmodule
