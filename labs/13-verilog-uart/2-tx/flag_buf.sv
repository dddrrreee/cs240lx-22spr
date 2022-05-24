module flag_buf
    #(
        parameter W = 8
    )
    (
        input logic clk, rst,
        input logic clr_flag, set_flag,
        input logic [W-1:0] din,
        output logic flag,
        output logic [W-1:0] dout
    );

    logic [W-1:0] buf_reg, buf_next;
    logic flag_reg, flag_next;

    always_ff @(posedge clk, posedge rst) begin
        if (rst) begin
            buf_reg <= 0;
            flag_reg <= 1'b0;
        end else begin
            buf_reg <= buf_next;
            flag_reg <= flag_next;
        end
    end

    always_comb begin
        buf_next = buf_reg;
        flag_next = flag_reg;
        if (set_flag) begin
            buf_next = din;
            flag_next = 1'b1;
        end else if (clr_flag) begin
            flag_next = 1'b0;
        end
    end

    assign dout = buf_reg;
    assign flag = flag_reg;
endmodule
