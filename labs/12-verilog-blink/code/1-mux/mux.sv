module mux
    (
        input logic a, b,
        input logic sel,
        output logic out
    );
    // TODO
    assign out = (!a & sel) | (b & sel);
endmodule
