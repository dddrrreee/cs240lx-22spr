extern fn nop() void;

// countdown 'ticks' cycles
pub fn delay(ticks: u32) void {
    var cnt = @as(u32, ticks);
    while (cnt > 0) {
        nop();
        cnt -= @as(u32, 1);
    }
}


