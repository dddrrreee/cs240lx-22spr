const gpio = @import("gpio.zig");
const rpi = @import("rpi.zig");

export fn zigMain() noreturn {
    const led_pin = .{ .num = 20 };
    // set the pin to output 
    gpio.gpio_set_output(led_pin);

    while (true) {
        // call set on 
        
        rpi.delay(1000000);
        
        // call set off 

        rpi.delay(1000000);
    }

    unreachable;
}
