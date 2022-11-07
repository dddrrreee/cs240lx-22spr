// Output q will count up to M and then reset to 0.
module counter
    #(
        parameter M = 10
    )
    (
        input  logic clk, rst,
        output logic [$clog2(M)-1:0] q
    );
    // always_comb begin
    always @(posedge clk, posedge rst) begin
        if(rst)
            q <= '0;
        else if(q == M)
            q <= '0;
        else
            q = q + 1;
    end
    
endmodule
