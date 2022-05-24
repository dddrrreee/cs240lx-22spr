// Output q will count up to M and then reset to 0.
module counter
    #(
        parameter M = 10
    )
    (
        input  logic clk, rst,
        output logic [$clog2(M)-1:0] q
    );
    // TODO
endmodule
