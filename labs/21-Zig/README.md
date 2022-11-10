# Zig lab
<hr>
<<<<<<< HEAD
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
=======
<b>Before we start, Zig is an UNSTABLE language still. It is only on version 0.10.0, and will not be considered stable until its 1.0.x release. I estimate another 2 years probably, which would make Zig about 10 years old, a similar age to when Rust stabalized itself.</b>
<br>
<b>That being said, Zig is ripe for contributions, and if you like any of what you see or have cool ideas for ways to apply some of the things Zig wants to offer, you should consider contributing! They accept a lot of RFC's, so go nuts with it if you want, or just go bug hunting and submit (hopefully helpful) bug issues for them to tackle.</b>
<hr>
<h2> Table of Contents</h2>

1. [Preface](https://github.com/TristenSeth/CS240LX-spr22/blob/master/labs/21-Zig/README.md#preface)
2. [Compiler Aside](https://github.com/TristenSeth/CS240LX-spr22/blob/master/labs/21-Zig/README.md#aside-about-the-zig-compiler-v0100-and-others)
3. [Setup](https://github.com/TristenSeth/CS240LX-spr22/blob/master/labs/21-Zig/README.md#setup)
4. [Lab](https://github.com/TristenSeth/CS240LX-spr22/blob/master/labs/21-Zig/README.md#lab)
5. [Extensions](https://github.com/TristenSeth/CS240LX-spr22/blob/master/labs/21-Zig/README.md#extensions)
6. [Notes/Highlights on Zig](notes.md)
</ol>
<h2>Preface</h2>

First, I will try to answer the elephant in the room, then I will give you all the prose for the lab.

<h4><b>Why Zig?</b></h4>
<details close>

There are many things Zig aims to accomplish (as most programming languages seem to do...). However, one that I find interesting is its committment to
interoperability with C/C++ code, so that you can maintain C/C++ legacy code bases, while also building on top of it with Zig as you go forward. We will
use this feature today.

The Zig language team has also expressed a strong desire to include embedded systems people from the start. So, unlike Rust, I hope that Zig is able to become as dead-simple to get working on a new machine as C is, while also giving us some niceties thanks to advancments in PL development and theory. 

Also, learning Zig is much faster (at least for me) than learning Rust. This isn't an attack on Rust. I have contributions to a [well known embedded OS](https://github.com/tock/tock) written in Rust, and really love the language. However, their idea of supporting embedded systems only extends to micro-controllers, ideally some sort of Cortex-M based system. While that is necessary and cool, I personally want to be able to use the full capabilities of my SBC (Small Board Computer) with entirely my own codebase. I would love to use Rust, but they make it entirely too difficult to get off the ground with a compilation target that is not officially supported. In that same vein, Zig is much simpler to get stuff done. For our pi, we aren't exactly trying to ship our code to millions of users, or even provide it as open source software, so to me, simple = better, and so I think we have less need for Rust here. 

Here is an example of a large project being built in Zig :

[Bun, a JS runtime meant to compete with Node.JS and Deno.JS with much better performance.](https://bun.sh)

[Media coverage of Bun](https://www.makeuseof.com/what-is-bunjs-why-the-javascript-community-excited/)

Here are some other cool zig projects:

[Boehm's style leak detector via a wrapper allocator, but in Zig](https://github.com/suirad/Seal)

[List of 'awesome' things being built with Zig](https://github.com/nrdmn/awesome-zig)

</details
<h4>Prose</h4>
<details close>

We are going to be trying out Zig for bare-metal programming today. Zig is a "general-purpose programming language and toolchain for maintaining robust, optimal and reusable software." according to the zig language [website](https://ziglang.org). I recommend poking aorund the language reference for v0.10.0, and taking note of anything you may want to try out. Zig gives a lot of functionality out of the box even in a freestanding environment. For example, take a look at the async/await functionality of the language. Notice how it does not have host dependencies? Well, then we can use it on our pi! Can you use this to write a single uart driver that supports asynchronous send/recv when available, as well as serial send/recv? Specifically, a single API that is agnostic to whether it is called asynchronously or not. Zig supports exactly this! If you want to test this, you can link into libpi to use our threads/processes/etc to actually make use of the async functionality, but be sure you use the -Dstage-1 compile flag so that you're compiling with the bootstrap compiler, since the self-hosted does not support async/await yet. If you still have issues, try using v0.9.1 of the compiler.

</details>
<hr>
<h3>Aside about the Zig compiler v0.10.0 and others</h3>
<details close>

Zig only has one compiler (well, I think someone is building a version of the Zig frontend completely in C...but why?) and it is not even stable yet. For this reason, there are some major differences between Zig compiler versions. The only version we will care about today is the latest tagged release, which is v0.10.0 . We can still access v0.9.1's compiler even in v0.10.0 thanks to the -fStage-1 flag.

This version just released last month, and now the master branch is busy working toward v0.11.0 . The primary focus of v0.10.0 was to bring up the self-hosted compiler (throw back to trusting trust). The bootstrap compiler (referred to as stage-1 by Zig docs) for Zig was written in C++, and took a TON of memory to build itself (you needed 32Gb of memory minimum), and there were things in the Zig language spec that the bootstrap compiler could not adequately comform to due to the way C++ is built. 

Now, we officially have a self-hosted compiler. It now takes like 2.9Gb to build the Zig compiler with itself on my machine, so this is a huge step for making zig more contributor friendly (b/c i know i didnt have a 32Gb laptop to build the compiler with). With that being said, there are bound to be many new bugs with it. For example, I have seen that the debug build of a GPIO blink program for our pi was only 2Kb when compiled with the v0.9.1 compiler, which is still a stage1 compiler. However, with the self-hosted stage2 compiler, the binary size is 20Kb, so 10x the size. Both binaries work, and both will compile down to a 136 byte binary on fast or small builds. It is unclear to me if this inflation is a "bug" or just the compiler actually being able to handle and include all of the debugging and safety hints that Zig is meant to offer via its language spec. I have a hunch though that it is simply emitting worse IR for LLVM in some way, so LLVM cant optimize as much. This could be beneficial for us if it means less changes between what we actually write and the machine code the compiler will actually emit. Some testing may be worth while...

The stages of the zig compiler are as follows:

<b>Stage-1</b> : bootstrapped compiler, written in C++ to compile .zig source files into LLVM IR, and passing that to LLVM for code generation.

<b>Stage-2</b> (We are here) : Self-hosted compiler, written in Zig (in .zig source files, not .cpp), which compiles .zig source files down into LLVM IR for code gen.

<b>Whats next?</b>

Key milestones for the v0.11.0 release are:

1. Package manager for Zig and C (maybe C++) packages too. Idea is to make the Zig toolchain the best C/C++ toolchain to use, even if you never write Zig.
2. Self hosted linker. Right now the default is lld, which is LLVM's linker. Zig wants to remove all of its dependencies on C++ and LLVM by default. Currently the self hosted linker works on most things, but not ELF files by default. Apparently the Mach-O stuff is neat from what I have heard.
3. Code generation backend. The Zig team wants to roll their own code gen backend, which could be used by other language front-ends, but mainly removes Zig dependency on LLVM and C++ (and Cmake, zlib, etc). While I'm not too confident they will beat LLVM code generation for optimizations, who knows. Maybe they will, but at the very least it can help prevent compiler backends from becoming a huge mono-culture of everyone using LLVM. 

    - Zig plans to do this with several kinds of backends: WebAssembly backend, C backend, as well as x86, arm, and aarch64 backends. Basically, you can tell the compiler which backend to use (or it determines it based on the target triple), and it will emit that kind of code. For the C backend, this means compiling your .zig source files would emit C code. For arm backend, it would emit arm machine code.
  
4. Remove the dependencies on C++, LLVM, etc for building a self-hosted zig toolchain (also known as bootstrapping). For this to happen, they need the C backend to be working, and luckily right now it passes (1259/1374) 92% of the behavior tests. This is so that the flow for building a self hosted compiler from bootstrapping (i.e. maybe you only have a C compiler on the machine) can become much easier. The flow becomes 
    - Use system C compiler to compile .c source files into zig1 executable
    - Use zig1 executable to compile .zig source files into .c source files
    - Use system C compiler to compile generated .c source files into zig2 executable
    - Use zig2 executable to compile .zig source files into zig executable

The above flow should sound familiar from our trusting-trust lab.
</details>
<hr>

## Prelab

#### Setup


Download the Zig toolchain [here](https://ziglang.org/download/), or use a [package manager](https://github.com/ziglang/zig/wiki/Install-Zig-from-a-Package-Manager). We want version 0.10.0, <b>Not master</b>. Once you extract the folder, move the 'zig' executable, <b>as well as</b> the lib/ directory into some place your shell can find it. Make sure you place the lib/ directory in the same directory as you place your 'zig' executable. 

Additionally, Zig 0.10.0 uses LLVM 15. Using a package manager i believe makes sure you have these dependencies, but I'm unsure about the prebuilt .zip's they release. If you're having problems, try making sure you have LLVM 15 installed and in a place your shell (and zig) can find it.

Aside: If you want, you can do like me and setup zig9/ and zig10/ directories, where each just contains the zig executable for that version, and the lib/ directory for that version. I then just made an alias for each in my shell rc file, and now I have easy access to both the v0.9.1 and v0.10.0 Zig compiler. It is nice for sniffing out whether something could be a miscompilation or bug on the Zig compiler's behalf, or if you're just doing something wrong (or maybe the data sheet is wrong). Just be careful because there WERE language changes between the two versions. If you do this, you will ALSO need LLVM 13 for the v0.9.1 compiler. I leave the choice up to you whether you care to install the previous version, but it does have a more "stable" experience. If you want to contribute to Zig, this could be helpful for regression testing and stuff.


Run 'zig version' (or if you have two compilers like I do, run 'zig9 version' and 'zig10 version') and you should get output like this:
<<<<<<< HEAD
=======
>>>>>>> d3b2ce371e6de231b529e14be51416585c2a798e
>>>>>>> merger

```
> zig version
0.10.0
```
<<<<<<< HEAD
=======
<<<<<<< HEAD
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
=======
>>>>>>> merger

#### Warming up with Zig

I would follow read [this](https://ziglearn.org/chapter-0/) guide to get familiar with Zig. Maybe follow along, or just read it. It is a fairly compact intro to the language (at least compared to the Rust book). You can skip the last chapter on Async, since the self hosted does not support that yet, but if you want to explore it with the stage-1 compiler then go ahead and take a look.

There is also a set of [neat exercises](https://github.com/ratfactor/ziglings) you can do. I would recommend just doing the early ones and then whichever ones seem most interesting (given that there are 90 exercises, i dont really expect any of you to have the time to do them all, even though they are VERY simple)

Also take a look at the notes I provided as well. They may not be much help, and they definitely are not critical analysis of the language in any way. But it is a convenient collection of stuff I have learned about the language from various places like their site, documentation, various blogs, and open source code I have looked at.

<hr>
<h2>Lab</h2>

1. Write a Blink with just Zig and asm.
2. Using Comptime to build a uart driver that does not allow for dumb developer mistakes, yet still takes up (approx) as little space in our binary as libpi's uart driver.
3. Link to our libpi static library and use our existing C code.
4. Use our libpi code to hook into the std lib (i.e. create a Writer that uses our uart, so we can then use all of the std lib's Writer types (bufferedWriter, countWriter, etc).


<h2>Extensions</h2>

1. Build out the comptime idea with other drivers. This has the benefit of making the API harder to use incorrectly, and decreased binary size compared to runtime safety since we can use comptime to our advantage and make as many things zero-sized as possible. For more info on stuff like this, Will Chr
2. Try and use zig translate-c utility to translate our existing libpi code into Zig code, see if it can compile and run it. I know this will be kind of gross since its auto-generated code, and what not, but could be cool.
3. Use the async/await methodology Zig provides for something cool on our pi. UART is definitionally asynchronous, so maybe that could be cool.
4. Use more of our libpi functionality to bring up more std lib features. Currently, only Mac, Windows and Linux have full std lib support, so it'd be cool to see how much we could get with our code cobbled together.
<<<<<<< HEAD
=======
>>>>>>> d3b2ce371e6de231b529e14be51416585c2a798e
>>>>>>> merger
