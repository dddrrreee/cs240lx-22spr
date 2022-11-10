# Highlights and Notes on Zig:
<hr>
<<<<<<< HEAD

=======
<<<<<<< HEAD
=======

>>>>>>> d3b2ce371e6de231b529e14be51416585c2a798e
>>>>>>> merger
## GENERICS
- types are values that have to be known at compile time.
- a generic data structure is just a function that returns a type.
- Like templates in C++

## COMPTIME
- has compile time reflection/introspection
- @typeinfo builtin provides type introspection at compile time

- 	Printing 
	- The Zig Standard Library uses this technique to implement formatted printing. Despite being a Small, simple language, 
	- 	Zig’s formatted printing is implemented entirely in Zig. Meanwhile, in C, compile errors for printf are hard-coded into the compiler. 
	- 	Similarly, in Rust, the formatted printing macro is hard-coded into the compiler.

## Zig as a C compiler
- ships with 40 versions of libc binaries
	- efficient use of importing C libraries via pulling in header files, and build artifact caching.
- can compile into static and dynamic libs.
- Can export types, functions, variables, from Zig for C code to depend on.


## Async
- async functions do not have any host dependencies, or heap allocation. <b>This means async functions are available for bare metal targets. </b>
- Zig infers whether a function is async, and allows async/await on non-async functions, which means that Zig libraries are agnostic of blocking vs async I/O. 
- Zig avoids [function colors](https://kristoff.it/blog/zig-colorblind-async-await/).

## Package manager
- plans for package manager, not just for zig libraries, but C ones as well.
- Now that self hosted compiler is released, this task is coming into scope. Lots could be done here!

## Draw backs
- not stable yet, still 1-2 years from 1.0 at least
- just shipped first self-hosted compiler, has bugs
- lack of package manager now

## Niceties
- Very open to change still
- heavy interest in embedded systems and OS dev working well with zig
- very active community already
- plans for package manager would push it over the edge as a C dev environment
- small, easy to learn for a C dev
- gives tool to both shoot yourself in foot, but also gives you the tools to make sure you dont do it twice.

## General Notes
<details open>

- 	Aims to be small, and simple 
	-  focus on debugging your code, not your knowledge of the language
	- Entire grammar is in a 500 line PEG Grammar file
- No hidden control flow, memory allocations, no preprocessor, no macros.
	- If Zig doesn’t look like its jumping around doing something, then it isn’t. 
-  Performance & Safety
	- 4 build modes
	- Can be mixed and matched all the way down to scope granularity. ( @setRuntimeSafety() )
	- two options are optimizations & runtime safety checks
- Stacktraces in debug builds work for ALL targets, including freestanding
- Zig does automatic Link time optimization
- Robust undefined behavior
	- Ex. Both signed and unsigned ints have clearly defined undefined behavior like overflows, but in C, only signed integers have undefined behavior on overflow
<<<<<<< HEAD
- Direct exposure of SIMD Vector types	
- NOT FULLY MEMORY SAFE, but this is not Rust, and does not aim to be. It aims to compete with C for ease of development, and blazing speed, but with nicer Programming lang theory stuff added in from the past 50 years to make dev experience nicer
=======
<<<<<<< HEAD
- Direct exposure of SIMD Vector types	- NOT FULLY SAFE, but this is not Rust, and does not aim to be. It aims to compete with C for ease of development, and blazing speed, but with nicer Programming lang theory stuff added in from the past 50 years to make dev experience nicer
=======
- Direct exposure of SIMD Vector types	
- NOT FULLY MEMORY SAFE, but this is not Rust, and does not aim to be. It aims to compete with C for ease of development, and blazing speed, but with nicer Programming lang theory stuff added in from the past 50 years to make dev experience nicer
>>>>>>> d3b2ce371e6de231b529e14be51416585c2a798e
>>>>>>> merger
- Zig std lib CAN integrate with libc, but does not depend on it.
- Global scope declarations (like variables) are order-independent, lazily evaluated, and initialized at compile time.
- standard ptrs can NOT be null
	- has optionals instead. 
	- any type can be made an optional, which can be something of type T, or null.
- Errors are values, cannot be ignores (must catch them using catch or try)
	- Error return traces != stack traces. The code does not have to pay the unwinding stack penalty for the error return trace.
- switch statement similar to Rust’s match, can ensure all possible errors get handled.
- ‘unreachable’ keyword is used to specify when a block of code should never be reached. 
	- In debug and release-safe, Reaching this statement will force a call to panic with a message that we reached unreachable code.
	- in optimized builds the compiler relies on these statements to be true to make optimizations

<<<<<<< HEAD
</details>
=======
<<<<<<< HEAD
</details>
=======
</details>
>>>>>>> d3b2ce371e6de231b529e14be51416585c2a798e
>>>>>>> merger
