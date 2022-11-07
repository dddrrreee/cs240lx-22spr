module adder
    (
        input logic [31:0] a, b,
        output logic [31:0] sum
    );
    logic [32:0] c;

    half_adder half_addr_u (.c(c[0]), .s(sum[0]), .a(a[0]), .b(b[0]));
    generate
        genvar i;
        for(i = 1; i <= 31; i = i + 1) 
            full_adder addr_u (.cout(c[i]), .s(sum[i]), .a(a[i]), .b(b[i]), .cin(c[i-1]));
    endgenerate

endmodule

module half_adder
    (
        input logic a, b,
        output logic c, s
    );
    // TODO
    assign c = a & b;
    assign s = a ^ b;
endmodule

module full_adder
    (
        input logic a, b, cin,
        output logic cout, s
    );

    assign cout = (a & b) | (a & cin) | (b & cin);
    assign s = a ^ b ^ cin;
endmodule
