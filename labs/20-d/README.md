# D lab

In this lab we'll try out using [D](https://dlang.org) for bare-metal
programming on the Raspberry Pi. D is a systems programming language that was
first released in 2001, trying to be a more sane alternative to C++. The
language went through a major upgrade from D1 to D2, completing in 2010. D has
three primary compilers:

* [DMD](https://github.com/dlang/dmd) (Digital Mars D): the reference compiler. Only targets amd64 and i386 (so
  we will not be using it).
* [GDC](https://www.gdcproject.org/) (GNU D compiler): this is the D compiler provided as part of GCC.
* [LDC](https://github.com/ldc-developers/ldc) (LLVM D compiler): a D compiler using LLVM as the backend.

Today's lab can be done with LDC or GDC (just change a build flag), but the
default is LDC because it is easier to install.

## Prelab

### Install a D compiler

You can choose whether you'd like to use GDC or LDC. The final compiler you
have installed should be called either `arm-none-eabi-gdc` or `ldc2`.

A GNU toolchain with a D compiler can be built by passing
`--enable-languages=d` to the GCC builder. I have some pre-built GNU D
toolchains for bare-metal ARM, Aarch64, and RISC-V at
[github.com/zyedidia/build-gdc](https://github.com/zyedidia/build-gdc/releases/tag/prebuilt).
If you want to use GDC and have Linux you can download one of those. They are a
GCC 10.3.0 toolchain.

LDC is available at
[github.com/ldc-developers/ldc](https://github.com/ldc-developers/ldc), and
they have prebuilt binaries
[here](https://github.com/ldc-developers/ldc/releases/tag/v1.30.0). Download
the one for your laptop (they all have cross-compilation built-in). You'll also
need a normal GNU toolchain for linking, objcopy, objdump, etc., but you should
already have this from all the previous labs.

#### Building GDC from source

If you want/need to install GDC from source you can use the gdc builder script at https://github.com/zyedidia/build-gdc. Run the following commands:

```
git clone https://github.com/zyedidia/build-gdc
cd build-gdc
git submodule update --init
CC=gcc ./build.sh arm-none-eabi
```

The build should place the resulting binaries in `./gnu-arm-none-eabi`.

### Install Knit

Yes the build system will use Knit. You can get a prebuilt binary for version
0.2.0 from [here](https://github.com/zyedidia/knit/releases/tag/v0.2.0).

### Install Qemu (optional)

[Qemu](https://www.qemu.org/download/) lets you run programs for the Pi without needing it to be plugged in, which can be quite useful. You can also use GDB
with Qemu for some in-depth debugging. Qemu is not necessary for the lab, but
the build scripts will support it if you want to use it.

### Check your installation

If you can successfully run `knit` in this directory, then you should be good
to go.

### Try out D

Normally D has a runtime with a garbage collector, classes with type info and a
bunch of other stuff. It also has a mode called "betterC" where the runtime is
disabled, and it is intended to be used just with a C runtime/standard library.
We will be using betterC with no C runtime/standard library (bare-metal).

You can familiarize yourself with D by going through the [D
tour](https://tour.dlang.org/). It has several chapters, each with several
sections. I recommend looking at the following:

* chapter 1, sections 1-12 (basic syntax and features).
* chapter 1, sections 17-18 (templates and delegates).
* chapter 2, sections 1-2 (interesting features)
* chapter 2, sections 5-15 (more advanced features).
    * The most important things for us here are: compile time function
      evaluation, template meta-programming, traits, attributes, and bit
      manipulation, but it's also useful to know that D supports things like
      contract programming (e.g., function pre- and post-conditions).
* chapter 3, section 1 (thread-local storage, useful to know that D does this
  for globals).

## Understanding the code organization

In the `code` directory, we have several sub-directories:

* `kernel`: kernel code, with implementations for GPIO/UART, and architecture
  specific things like device barriers and delay functions.
* `libd`: a small library that is our replacement for the D standard library.
* `arch`: architecture-specific startup assembly.
* `board`: board-specific linkerscript.
* `progs/*`: example programs. Each one has a function `kmain` which is the
  entrypoint called after starting up and initializing the UART.

### Mini D library

When we use D in betterC mode, there is no longer any D runtime or standard
library, so we have to write the library ourselves. Our mini D library is in
`libd`. It has a file called `object.d` that is automatically imported
everywhere, which defines some useful types like `uintptr`, `size_t`, and
`string`. Depending on the features of D that we use, the compiler may also
make calls to various internal library functions, which we must now define
ourselves. One example is `_d_arraycopy` (or `_d_array_slice_copy` in LDC), which
is called when assigning a string to a slice.

Some more internals are defined in `core`, and `gcc`:

* `core/bitop.d`: defines `volatileStore` and `volatileLoad`, which are
  compiler intrinsics for loads and stores that are guaranteed to not be
  removed or reordered
  (https://dlang.org/library/core/volatile/volatile_store.html).
* `core/exception.d`: defines internal functions that the compiler calls when
  there are assertion failures and bounds check failures.
* `gcc/*`: imports builtins and attributes for the GDC compiler.

The main part of the mini D library is in `ulib` and defines helpers for bit
manipulation, memory operations, meta-programming, and more (including things
we won't need for the lab, but might be interesting to look at). In part 2
of the lab you'll write utilities for printing in `io.d`, and in part 3 you'll
write a typed allocator in `alloc.d`.

### Kernel code

The `kernel` directory has library code for writing bare-metal programs. It
uses conditional compilation to import different packages depending on the
architecture and board we are compiling for. The `arch` directory contains
architecture specific definitions for memory barriers, and built-in timers. The
supported architectures are currently `arm`, `aarch64`, and `riscv32`. The
`board` directory contains drivers for various boards for GPIO, UART,
rebooting, and system information (clock frequency). Currently I just have code
for the r/pi 1 A+, r/pi 3, and r/pi 4b, but other boards could easily be added
(for example HiFive rev B, especially since `arch` already supports `riscv32`).

## Building

Running `knit` from anywhere in the project will build the programs in `progs`.

From inside a particular `progs/x` directory there are several directory-specific targets:

* `knit install` will build it and run `pi-install`.
* `knit qemu` will build+run with qemu.
* `knit qemu-gdb` will build+run with qemu but drop you into a GDB instance for
  debugging.
* `knit all` will build the bin/list for just `x`.

In general here are some cool things:

* `knit [TARGET] -t clean` will automatically clean outputs produced by just
  that target.
* `knit [TARGET] -t status` will show you the build status for that target.
* `knit [TARGET] -t graph pdf > graph.pdf` will output a graph of the build.

See `knit -t list` for more sub-tools that Knit supports.

You can modify `conf.lua` to set your compiler to `gdc` or `ldc`, and change
your board to `raspi3b` or `raspi4b` (if you have one, or you can just use
qemu). You can also set these variables at the command line: `knit dc=gdc
board=raspi3b`.

## Part 1: re-implement blink in D

Convert your C GPIO driver to D in `kernel/board/raspi/gpio.d`. It should be
almost identical to the C version, because the two languages are quite similar.

Write a blink program in `progs/blink/main.d` to make sure it works.

Hints:

* Use `mmio.ld` and `mmio.st` (defined in `kernel/mmio.d`) for mmio operations.
* Use `enum` to define a "manifest constant"
  (https://wiki.dlang.org/Declaring_constants).
* `kernel/board/raspi/device.d` defines the MMIO base address depending on the
  type of Raspberry Pi you are targeting.

## Part 2: implement a basic print function

A UART driver is already written for you in `board/raspi/uart.d`. Now you can
use it to write a print function in our mini D library in `libd/ulib/io.d`.

The stub code defines a struct `File` that has a `putc` callback and implements
a writer on top of it. The `File.write` function is variadic and should call
the appropriate internal write function for each element passed in.

Run the `progs/hello/main.d` program to make sure it works.

I would recommend reading
https://dlang.org/blog/2020/07/31/the-abcs-of-templates-in-d/ to learn more
about templates, which are minimally used in this part and used more in the
next part.

Hints:

* Use the `itoa` function from `ulib/string.d` for converting an integer to a
  string.
* Types have various properties that are listed here:
  https://dlang.org/spec/property.html.
    * In particular, `T.sizeof` is number of bytes needed to store a value of
      type `T`.

### Extension: buffered writer

You can try implementing a buffered writer so that `write` will write to some
internal buffer and accumulate multiple writes together before flushing. This
won't really help performance for this particular case, but if we had a
bulk-write target (not `putc`), it would help to accumulate small writes into a
block before writing.

Hints:

* A slice's underlying data and length can be accessed with `.ptr` and `.length`:

```
T[] x;
x.ptr    // pointer to data
x.length // length of slice
```

* D has built-in slicing with `x[lo .. hi]` syntax. Use `$` to indicate start
  or end: `x[lo .. $]`.

## Part 3: typed allocator

Usually the D runtime provides built-in allocation, but we are using betterC,
so we have to provide our own allocator.

The file `libd/ulib/alloc.d` has a basic implementation of a bump allocator.
`Bump!N(base, size)` constructs a bump allocator that allocates memory blocks
aligned to a multiple of `N`, in the range `[base, base+size)`. It is not
type-safe because it returns uninitialized void pointers. Your task is to
implement `Allocator(A)`, which wraps an unsafe allocator `A` (like `Bump`, but
you could have another underlying allocator) with type-safe allocation
functions:

* `T* make(T, Args...)(Args args)`: allocates a new object of type `T`, and
  initializes it with the default constructed value for `T` (`args` is passed
  to `T`'s
  constructor). Returns a pointer to `T` or null if there isn't enough space.
* `void free(T)(T* val)`: destructs and frees a value allocated by `make`.
* `T[] makeArray(T, Args...)(size_t nelem, Args args)`: allocates an array of
  `T` of size `nelem`, and initializes each element. `args` is passed to `T`'s
  constructor.
* `void free(T)(T[] arr)`: destructs and frees an array allocated by
  `makeArray`.

Note a struct type can have default values given by member initializers:

```
struct Foo {
    int x = 7;
}
```

and also can have a constructors/a destructor:

```
struct Foo {
    int x;
    // constructor
    this(int x) {
    }
    // destructor
    ~this() {
    }
}
```

Structs allocated on the stack will automatically be constructed/destructed by
the compiler.

Run `progs/alloc/main.d` to make sure your allocator is working.

Hints:

* Types have various properties that are listed here:
  https://dlang.org/spec/property.html.
    * In particular, `T.init` gives the initial value for a type `T`.
* The constructor for type `T` can be accessed with `T.__ctor()`.
* The destructor for type `T` can be accessed with `T.__dtor()`.
* The compiler has several traits that can be accessed at compile time with the
  internal `__traits()` function. See https://dlang.org/spec/traits.html for
  documentation. GDC may not support all the traits listed there because it
  supports an older version of D.
    * `__traits(hasMember, T, x)` can be used to check if type
      `T` has a member function called `x`.
    * `__traits(initSymbol, T)` returns a `void[]` pointing to the default
      value for `T`. This trait is not supported by GDC. How is this
      different/better than `T.init`?
* In a compile-time check you can use the `is` expression to check properties
  about a certain type. For example `is(T == struct)` is true if `T` is a
  struct type. See https://wiki.dlang.org/Is_expression.

### Extension: better underlying allocator

Write a KR allocator (you should already have a C version) so that your
allocator can free memory too.

## Part 4: exploration (optional)

Some ideas:

* Explore more of D's compile-time function evaluation.
* Learn about `@safe`, `@trusted` and `@system`: function attributes that can
  be used to make unsafe operations like casts into errors.
* Learn about D's support for function contracts (pre- and post-conditions).
* Learn about pure functions.
* Write a function that fails a bounds check and make sure it causes an
  assertion failure automatically.
* Read through the Knitfile.
