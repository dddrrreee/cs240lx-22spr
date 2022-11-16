# Prelab: Memcheck in LLVM
 
## Recap
 
We have implemented two techniques to catch memory related bugs:
 
1. [9-memcheck-stat:](https://github.com/dddrrreee/cs240lx-22spr/tree/main/labs/9-memcheck-stat) Track every malloc and free, and interrupt the running process at regular intervals. If the current instruction is a load or store and within a specified range, create a trampoline that calls a checking function and runs the load or store instruction.
2.  [11-memcheck-trap:](https://github.com/dddrrreee/cs240lx-22spr/tree/main/labs/11-memcheck-trap) Using virtual memory, trap on every load and store to the heap. Check whether the load or store is legal. If legal, change permissions so that the instruction can be run and set a mismatch breakpoint so that permissions can be revoked after running that single instruction. If  illegal, throw an error.
 
Although the first technique required no changes to the source code and little other infrastructure besides the interrupts, we miss loads and stores because we depend on the frequency of timer interrupts. And, if you remember, dynamically generating code to insert trampolines is not very straightforward because some loads and stores are pc-relative. In the second technique, we do not miss any loads or stores or need to deal with dynamic code generation. But, it requires a lot of infrastructure (e.g. VM, interrupts, breakpoints).
 
## Another Approach
 
In this lab, we use a third technique:
 
3. Rewrite the binary to branch to functions after calls to malloc and free and track what memory has been allocated. Instrument every load and store to check whether loading or storing to that address is legal. If legal, continue execution, otherwise throw an error.
 
This way we do not change the source code, do not miss any load or store and do not need to set up costly infrastructure. But, static rewriting is hairy and error-prone, and you certainly don't want to do it from scratch on your own. Adding instructions means that direct branches need to be recalculated and pc-relative loads and stores need to be adjusted. LLVM does the instrumentation at compile time as a compiler pass and deals with all of the hairy rewriting details: we only need to provide the checking logic!

 
## About LLVM

The best introduction to LLVM is [here](https://www.aosabook.org/en/llvm.html) written by Chris Lattener, the original author of the LLVM project. You should also take a look at the [homepage](https://llvm.org/) of the LLVM project.

LLVM has a lot of interesting applications&mdash;static instrumentation, optimizations, program analysis&mdash;that may be interesting to folks who are not directly involved with compilers. Really, the goal of this lab is to convice you that LLVM is interesting and useful!

## Downloading Cmake and LLVM

I am using LLVM version 14.0.6 and cmake version 3.24.3. Unfortunately, LLVM is infamous for not being backwards compatible. For the smoothest experience, I would recommend getting the same versions as me. I will test with newer versions before lab and will help debug any problems that arise during lab. That way, we get the true LLVM experience! Two things to download:

1. Download cmake from precompiled binaries [here](https://cmake.org/download/). Apt and Homebrew commands for Ubuntu and Mac available [here](https://cgold.readthedocs.io/en/latest/first-step/installation.html).
2. Download LLVM. Prebuilt binaries/buidling from building from source links are [here](https://releases.llvm.org/download.html). Homebrew formula for Mac available [here](https://github.com/Homebrew/homebrew-core/blob/master/Formula/llvm.rb). You can also build LLVM using cmake, instructions [here](https://llvm.org/docs/CMake.html). Fortunately, we do not need to configure LLVM with any obscure options to get our project running. Follow the defaults.

Before, coming to lab, you should know where the following files are located:

1. The clang binary that comes with LLVM. For me this is path_to_llvm/llvm@14/bin/clang. 
2. The cmake directory that comes with LLVM. You can get this by calling ```llvm-config --cmakedir```. The ```llvm-config``` executable is in the same place as the clang binary. For me, this is path_to_llvm/llvm@14/14.0.6/lib/cmake/.

If you run into any problems with downloading, please text or email me!





 

