extern fn uart_init() void;
extern fn uart_disable() void;
extern fn uart_getc() i32;
extern fn uart_putc(c: u8) void;
extern fn uart_getc_async() i32;
extern fn uart_has_data() i32;
extern fn uart_can_putc() i32;
extern fn uart_flush_tx() void;
extern fn clean_reboot() noreturn;

fn debug_print(message: [:0]const u8) void {
    for (message) |char| {
        uart_putc(char);
    }
}

export fn zigMain() noreturn { 
    debug_print("Printing from Zig with C!\n");
    clean_reboot();
}
