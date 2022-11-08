# Zig lab
<hr>
***Before we start, Zig is an UNSTABLE language still. It is only on version 0.10.0, and will not be considered stable until its 1.0.x release. I estimate another 2 years probably, which would make Zig about 10 years old, a similar age to when Rust stabalized itself.
<br>***
***That being said, Zig is ripe for contributions, and if you like any of what you see or have cool ideas for ways to apply some of the things Zig wants to offer, you should consider contributing! They accept a lot of RFC's, so go nuts with it if you want, or just go bug hunting and submit (hopefully helpful) bug issues for them to tackle.***
<hr>
## Table of Contents
<ol>
	<li>[Preface]()</li>
	<li>[Compiler Aside]()</li>
	<li>[Setup]()</li>
	<li>[Notes/Highlights on Zig]()</li>
</ol>
### Preface
We are going to be trying out Zig for bare-metal programming today. Zig is a "general-purpose programming language and toolchain for maintaining robust, optimal and reusable software." according to the zig language [website](https://ziglang.org). There are many things Zig aims to accomplish (as most programming languages seem to do...), but one that I find interesting is its committment to interoperability with C/C++ code, so that you can maintain C/C++ legacy code bases, while also building on top of it with Zig as you go forward. The Zig language team has also expressed a strong desire to include embedded systems people from the start, so unlike Rust, I hope that Zig is able to become as dead-simple to get working on a new machine as C is, while also giving us many more niceties thanks to advancments in PL theory and computational power.

Zig only has one compiler (well, I think someone is building a version of the Zig frontend completely in C...but why?), and it is not even stable yet. For this reason, there are some major differences between Zig compiler versions. The only version we will care about today is the latest tagged release, which is v0.10.0 . 
<hr>
### Aside about the Zig compiler v0.10.0 and others
<details open>
This version just release last month, and now the master branch is busy working toward v0.11.0 . The primary focus of v0.10.0 was to bring up the self-hosted compiler (throw back to trusting trust). The bootstrap compiler (referred to as stage-1 by Zig docs) for Zig was written in C++, and took a TON of memory to build itself (you needed 32Gb of memory minimum), and there were things in the Zig language spec that the bootstrap compiler could not adequately comform to due to the way C++ is built. Now, we officially have a self-hosted compiler. It now takes like 2.9Gb to build the Zig compiler with itself on my machine, so this is a huge step for making zig more contributor friendly (b/c i know i didnt have a 32Gb laptop to build the compiler with). With that being said, there are bound to be many new bugs with it. For example, I have seen that the debug build of a GPIO blink program for our pi was only 2Kb when compiled with the v0.9.1 compiler, which is still a stage1 compiler. However, with the self-hosted stage2 compiler, the binary size is 20Kb, so 10x the size. Both binaries work, and both will compile down to a 136 byte binary on fast or small builds. It is unclear to me if this inflation is a "bug" or just the compiler actually being able to handle and include all of the debugging and safety hints that Zig is meant to offer via its language spec. I have a hunch though that it is simply emitting worse IR for LLVM in some way, so LLVM cant optimize as much. This could be beneficial for us if it means less changes between what we actually write and the machine code the compiler will actually emit. Some testing may be worth while...
</details>
<hr>
## Prelab

### Setup
<hr>
Download the Zig toolchain [here](https://ziglang.org/download/), or use a [package manager](https://github.com/ziglang/zig/wiki/Install-Zig-from-a-Package-Manager). We want version 0.10.0, <b>Not master</b>. Once you extract the folder, move the 'zig' executable, <b>as well as</b> the lib/ directory into some place your shell can find it. For me, i put all that stuff in ~/bin directory that i created. Make sure you place the lib/ directory in the same directory as you place your 'zig' executable.

Run 'zig version' and you should get output like this:

```
> zig version
0.10.0
```
Additionally, Zig 0.10.0 uses LLVM 15. Using a package manager i believe makes sure you have these dependencies, but I'm unsure about the prebuilt zips they release. If you're having problems, try making sure you have LLVM 15 installed and in a place your shell (and zig) can find it.

I would recommend trying to write a simple hello world program in Zig, and compiling it to make sure it is all working correctly. Dont get too caught in the build system yet, we will talk more about it in lab.

There is also a set of [neat exercises](https://github.com/ratfactor/ziglings) you can do. I would recommend just doing the early ones and then whichever ones seem most interesting (given that there are 90 exercises, i dont really expect any of you to have the time to do them all, even though they are VERY simple)
<hr>
## Lab
- 1. Write a Blink with just Zig and asm.
- 2. Building a typed-state device driver for the uart.
- 3. (maybe) Build an allocator to make Zig Std Lib available on our pi.
- 4. (maybe) Link to our libpi.a and use our existing C code for something.


## Extensions
- 1. Build out the type-state idea with other drivers. This makes it easy to continue building type-state userspace libraries.
- 2. Try and use zig translate-c utility to translate our existing libpi code into Zig code, see if it can compile and run it. I know this will be kind of gross since its auto-generated code, and what not, but could be cool.
- 3. Use the async/await methodology Zig provides for something cool on our pi. UART is definitionally asynchronous, so maybe that could be cool.