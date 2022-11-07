module ulib.bits;

import ulib.trait;

T mask(T)(uint nbits) if (isInt!T) {
    if (nbits == T.sizeof * 8) {
        return ~0;
    }
    return (1 << nbits) - 1;
}

T get(T)(T x, uint ub, uint lb) if (isInt!T) {
    return (x >> lb) & mask!T(ub - lb + 1);
}

bool get(T)(T x, uint bit) if (isInt!T) {
    return (x >> bit) & 1;
}

version (GNU) {
    import gcc.builtins;

    size_t msb(uint x) {
        return x ? x.sizeof * 8 - __builtin_clz(x) : 0;
    }

    size_t msb(ulong x) {
        return x ? x.sizeof * 8 - __builtin_clzll(x) : 0;
    }
}

version (LDC) {
    import ldc.intrinsics;

    size_t msb(uint x) {
        return cast(size_t)(x ? x.sizeof * 8 - llvm_ctlz!uint(x, true) : 0);
    }

    size_t msb(ulong x) {
        return cast(size_t)(x ? x.sizeof * 8 - llvm_ctlz!ulong(x, true) : 0);
    }
}
