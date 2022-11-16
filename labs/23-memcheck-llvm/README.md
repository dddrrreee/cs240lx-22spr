# Memcheck: LLVM Edition

Read the [PRELAB](PRELAB.md).

Today, we are going to write an LLVM pass that checks for memory corruption. The LLVM pass instruments every call to malloc and free, and logs which memory locations are safe to access. It also instruments every load and store and checks whether that memory access is safe using our log.

The code associated with this lab uses a [skeleton pass](https://github.com/sampsyo/llvm-pass-skeleton.git) written by [Adrian Sampson](https://www.cs.cornell.edu/~asampson/). When I began writing LLVM passes, I found Sampon's blog post [LLVM for Grad Students](https://www.cs.cornell.edu/~asampson/blog/llvm.html) extremely useful. Another useful and comprehensive reference is [Writing an LLVM Pass](https://llvm.org/docs/WritingAnLLVMPass.html) written by the LLVM folks themselves.

This README uses LLVM version 14.0.6 and cmake version 3.24.3. 



## LLVM documentation

The main documentation that you will need today are LLVM's [Doxygen](https://llvm.org/doxygen/) pages. Today, I will link against relevant pages, but productivity in LLVM boils down sifting through these pages efficiently.

If you're interested in understanding LLVM IR, you can read the [LLVM Language Reference Manual](https://llvm.org/docs/LangRef.html).

## Part 0: Building the Pass
Everything in this subsection is described relative to ```23-memcheck-llvm/part1```. 

A dummy "hello world" pass has been defined in ```pass/dummy_pass.cpp```. To build ```dummy_pass.cpp``` run:

```
mkdir build
cd build
cmake ..
make
```
If cmake complains that it cannot find the directory where ```LLVM-Config.cmake``` exists, set the ```LLVM_DIR``` environment variable to the cmake directory that comes with LLVM. If needed, look over the last section of the PRELAB to see how to find this path using ```llvm-config```. Now, you can build using:

```
# inside build directory
cmake .. -DLLVM_DIR=<path_to_directory>
make
```

Check that you can find a ```libDummyPass.so``` under ```build/pass```. 

Look over ```rtlib.c```. This files defines the behavior of the functions that that are inserted during the pass.

Now, set ```LLVM_BIN``` in the Makefile to the directory that houses the clang executable that comes with LLVM. You should be able to build the part1 code now using ```make``` (in the ```part1``` directory, not the ```build``` directory). 

Expected output after calling ```make```:

```
RTLIB: Hello from instrumented function!
Executing main.
```


## Part 1: Writing a Simple Pass

Currently, our dummy pass calls the ```print_hello``` function before executing any instructions in ```main```. 

Now, you will add code to ```pass/dummy_pass.cpp``` to call ```print_with_arg``` before calling ```test```. Look at ```rtlib.c``` for the definition of ```print_with_arg```. 

In ```dummy_pass.cpp```:

1. Add the FunctionType for ```print_with_arg``` function. You can find the definition of ```FunctionType::get``` on [this](https://llvm.org/doxygen/classllvm_1_1FunctionType.html) page.
2. Make a FunctionCallee for ```print_with_arg``` function similar to the ```print_hello``` function.
3. To instrument any calls to ```test```, we check whether the name of the function being called is "test" for all ```CallInst``` IR node ([link](http://formalverification.cs.utah.edu/llvm_doxy/2.9/classllvm_1_1CallInst.html)). Add code that gets the first argument of ```test```  and insert a call to ```print_with_arg```  passing in that argument. A method on the ```CallInst``` node  that returns the operand is defined [here](http://formalverification.cs.utah.edu/llvm_doxy/2.9/classllvm_1_1CallInst.html). To insert the call, follow the example of ```print_hello```. You will need to change the argument of one of the ```Builder``` functions to pass the argument to ```print_with_arg```.

Now, when you run make, you should see:

```
RTLIB: Hello from runtime Library!
Executing main.
RTLIB: Argument Passed = 42
Executing test with argument 42
RTLIB: Argument Passed = 11
Executing test with argument 11
```

## Part 2: Completing Memcheck
The memcheck pass is defined in ```part2/pass/memcheck_pass.cpp```. Look at ```part2/rtlib.c``` to see how the checking takes place. Here's what the logging functions do:

1. ```log_malloc```: This function gets called after a call to ```malloc``` and takes the pointer returned by malloc and the size of the memory requested as arguments. 
2. ```log_free```: This function gets called after a call to ```free``` and takes the pointer that is being freed as an argument. It also checks for any double free errors.
3. ```log_load```: This function gets called before a load instruction and checks whether the load is safe. Since our logging functions also call ```malloc``` and ```free```, we want to ensure that the user does not overwrite our memory! 
4. ```log_store```: This function gets called before a store instruction and checks whether the store is safe. 
5. ```log_stack```: To prevent false positives, we must also track stack memory. This function tracks the memory that is allocated to the stack. Users are not responsible for freeing this memory.
6. ```init_check```: This function gets called before any instructions in ```main``` are called. It sets up state for the rest of our logging function.
7. ```exit_check```: This function gets called after the last instruction in ```main```. It checks for any pointers that may not have been freed and frees any memory that has been allocated by the other logging functions.

The pass must do the following:
1. Call ```log_malloc``` after calls to ```malloc``` and pass the the pointer returned by ```malloc``` and the size of the requested memory as arguments. 
2.  Call ```log_free``` after calls to ```free``` and pass the pointer being freed as an argument.
3. Call ```log_load``` before a ```load``` instruction.
4. Call ```log_store``` before a ```store``` instruction.
5. Call ```log_stack``` before a ```alloca``` instruction which requests memory for the stack.
6. Call ```init_check``` before main.
7. Call ```exit_check``` after main.

What you will need to do today: 

1. Follow steps from Part 0 of the lab (make the ```build``` directory and add ```LLVM_BIN``` in the Makefile).
2. Fill in the code in ```part2/pass/memcheck_pass.cpp``` for the ```load```, ```store``` and ```alloca``` instructions. You will need to cast the address used in these instructions to a ```uint8 *```. The code for each instruction is more or less the same.

Don't forget to call ```make``` in the build directory after you are modify ```part2/pass/memcheck_pass.cpp```.

You can change ```TEST_SRC``` to ```no-errors```, ```use-after-free```, ```unsafe-load```, ```unsafe-store``` and ```mem-leak``` to see the pass in action.


## Extension 

Now, that you can write a LLVM pass, the possibilities are endless. You can now:

* Build [Pin](https://software.intel.com/content/www/us/en/develop/articles/pin-a-dynamic-binary-instrumentation-tool.html).
* Build [Eraser](https://rcs.uwaterloo.ca/~ali/cs854-f17/papers/eraser.pdf).
* Add [loop perforation](https://dl.acm.org/doi/pdf/10.1145/2025113.2025133).
* Build [Atom](http://www-leland.stanford.edu/class/cs343/resources/atom.pdf).
* Build [Vagrind tools](http://www-leland.stanford.edu/class/cs343/resources/valgrind.pdf).
* Add auto-vectorization.
* Add [superoperators](https://dl.acm.org/doi/pdf/10.1145/199448.199526).
* Add array bounds checking. 
* Add [strength reduction](https://en.wikipedia.org/wiki/Strength_reduction). 
* Add [loop unrolling](https://en.wikipedia.org/wiki/Strength_reduction). 
* Add taint checking.

