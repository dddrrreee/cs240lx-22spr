module ulib.io;

import ulib.string : itoa;
import ulib.math : min, max;
import ulib.trait : isInt, Unqual;

import sys = ulib.sys;

struct File {
public:
    this(void function(ubyte) putc) {
        this.putc = putc;
    }

    void write(Args...)(Args args) {
        // TODO
    }

    void flush() {
        // if you implement a buffered writer
    }

private:
    // call this function to write a byte
    void function(ubyte) putc;

    void writeElem(string s) {
        // TODO
    }

    void writeElem(T)(T* val) {
        // TODO
    }

    void writeElem(char ch) {
        // TODO
    }

    void writeElem(bool b) {
        // TODO
    }

    void writeElem(S)(S value, uint base = 10) if (isInt!S) {
        // TODO
    }
}

void write(Args...)(Args args) {
    sys.stdout.write(args);
    sys.stdout.flush();
}

void writeln(Args...)(Args args) {
    sys.stdout.write(args, '\n');
    sys.stdout.flush();
}
