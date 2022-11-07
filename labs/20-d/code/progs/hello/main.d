module hello.main;

import io = ulib.io;

extern (C) void kmain() {
    io.writeln("Hello world!");
    io.writeln("Address of kmain: ", &kmain);
    io.writeln("Number: ", 42);
}
