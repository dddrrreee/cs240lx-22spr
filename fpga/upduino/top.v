`define CLK_MHZ 48

module top
    (
        output wire led_red,
        output wire led_green,
        output wire led_blue,

        // output wire gpio_25,
        // input wire gpio_23,
        output wire serial_txd,
        input wire serial_rxd,

        output wire spi_cs
    );

    assign spi_cs = 1'b1;

    wire clk_sys;
    SB_HFOSC u_SB_HFOSC (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(clk_sys));

    wire led_r, led_g, led_b;

    `TOP top (
        .led_r (led_r),
        .led_g (led_g),
        .led_b (led_b),
        .tx (serial_txd),
        .rx (serial_rxd),
        .clk (clk_sys),
        .rst (1'b0)
    );

    SB_RGBA_DRV rgb (
        .RGBLEDEN (1'b1),
        .RGB0PWM  (led_g),
        .RGB1PWM  (led_b),
        .RGB2PWM  (led_r),
        .CURREN   (1'b1),
        .RGB0     (led_green),
        .RGB1     (led_blue),
        .RGB2     (led_red)
    );
    defparam rgb.CURRENT_MODE = "0b1";
    defparam rgb.RGB0_CURRENT = "0b000001";
    defparam rgb.RGB1_CURRENT = "0b000001";
    defparam rgb.RGB2_CURRENT = "0b000001";
endmodule
