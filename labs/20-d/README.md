# D lab

In this lab we'll try out using [D](https://dlang.org) for bare-metal
programming on the Raspberry Pi. D is a systems programming language that was
first released in 2001, trying to be a more sane alternative to C++. The
language went through a major upgrade from D1 to D2, completing in 2010. D has
three primary compilers:

* DMD (Digital Mars D): the reference compiler. Only targets amd64 and i386 (so
  we will not be using it).
* GDC (GNU D compiler): this is the D compiler provided as part of GCC.
* LDC (LLVM D compiler): a D compiler using LLVM as the backend.

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

### Install Knit

Yes the build system will use Knit. You can get a prebuilt binary for version
0.2.0 from [here](https://github.com/zyedidia/knit/releases/tag/v0.2.0).

### Check your installation

If you can run successfully `knit` in this directory, then you should be good
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
