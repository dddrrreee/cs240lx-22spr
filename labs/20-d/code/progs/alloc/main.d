module hello.main;

import sys = kernel.sys;

static import kernel;

import io = ulib.io;
import ulib.alloc;

struct Foo {
    int a;
    int b = 7;
    bool destroyed = false;

    this(int a) {
        io.writeln("constructed a foo");
        this.a = a;
    }

    ~this() {
        destroyed = true;
        io.writeln("destroyed foo");
    }
}

extern (C) extern __gshared uint _kheap_start;

extern (C) void kmain() {
    kernel.init();

    enum mb(int n) = n * 1024 * 1024;

    auto a = Allocator!(Bump!16)(cast(uintptr)&_kheap_start, mb!1);
    Foo* foo = a.make!Foo(42);
    io.writeln("made a foo! ", foo.a, " ", foo.b);
    a.free(foo);

    uint* i = a.make!uint();
    a.free(i);

    Foo[] foos = a.makeArray!Foo(10, 42);
    io.writeln("made ", foos.length, " foos");
    a.free(foos);

    sys.reboot();
}
