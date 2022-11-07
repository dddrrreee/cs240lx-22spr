module ulib.alloc;

import ulib.memory;

struct Bump(size_t alignment = 16) {
    static uintptr align_off(uintptr ptr, size_t algn) {
        return ((~ptr) + 1) & (algn - 1);
    }

    this(uintptr base, size_t size) {
        // assert no overflow
        assert(base + size >= base);
        this.base = base;
        this.end = base + size;
        assert(this.base % alignment == 0);
        assert(this.end % alignment == 0);
    }

    void* allocPtr(size_t sz) {
        assert(sz + align_off(sz, alignment) >= sz);
        sz += align_off(sz, alignment);
        assert(base + sz >= base);
        if (base + sz >= end) {
            return null;
        }

        void* ptr = cast(void*) base;
        base += sz;
        return ptr;
    }

    void freePtr(void* ptr) {
        // no free
    }

private:
    uintptr base;
    uintptr end;
}

struct Kr(size_t alignment = 16) {
    this(uintptr base, size_t size) {
        bump = BumpAllocator(base, size);
    }

    void* allocPtr(size_t sz) {
        // TODO (extension)
        return null;
    }

    void freePtr(void* ptr) {
        // TODO (extension)
    }

private:
    // internal bump allocator for the KR allocator
    Bump bump;
}

// Emplaces the initial value for T into val. If T has a constructor, also
// calls the constructor with 'args'.
void emplaceInit(T, Args...)(T* val, Args args) {
    // TODO
}

// A typed allocator wrapping a basic allocator A. A must have the methods
// allocPtr and freePtr.
struct Allocator(A) {
    this(uintptr base, size_t size) {
        allocator = A(base, size);
    }

    T* make(T, Args...)(Args args) {
        // TODO
        return null;
    }

    void free(T)(T* val) {
        // TODO
    }

    T[] makeArray(T, Args...)(size_t nelem, Args args) {
        // TODO
        return null;
    }

    void free(T)(T[] arr) {
        // TODO
    }

private:
    A allocator;
}
