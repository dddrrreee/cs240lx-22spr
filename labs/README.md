## Labs.

This is a partial set of possible labs.  They've been split into ones
we'll almost certainly do and those that we might do depending on
interest.  I'm going through old notes and TODO files pulling in more,
so check back :)

There are way more possible labs than class slots so if you can please
look through these before the first class and see which you prefer.

In either case, these are a ***tentative*** set of labs.  They will
change!  Some will get dropped.  Some will get added.  All will get
modified significantly.

This is the second offering of the class.  While we have done many of
the labs once before in cs240lx or cs340lx many ideas are research-paper
level and how to best implement them is still up in the air, so the only
constant will be change.

----------------------------------------------------------------------------
##### low level code module

  - [1-dynamic-code-gen](1-dynamic-code-gen/README.md): you'll learn
    how to generate executable machine code at runtime and how to 
    use this trick to do neat stuff.  

    Possible things to build:
      - generate exact cycle write schedules where you give a set
        of reads and writes and the cycle they should occur at.
      - do real generic routines in C where you "curry" a parameter
        into a runtime thunk that calls the original routine.  this
        for example lets you replace `int foo(a,b,c)` with `int fp()` and, thus
        have a set of data structure routines that can work on routines
        that take no parameters and return an `int` rather than needing to 
        make (for example) an iterator routine for each different 
        data-structure.
      - simple packet filter JIT engine --- these are being heavily used
        in linux for monitoring.  could do BPF (what they use) or our own.
      - 


  - speed stuff [below]: open question: can be enable caching when no
    VM?  Not sure where to place this.

MAYBE:
  - linker script hacks.
  - bootloader that can copy itself out of the way + use this to 
    make a network bootloader.
  - a runtime code gen system that can work on riscv, x86, arm,
    etc.

---------------------------------------------------------------------
### Simple binary-analysis tools.

As we showed in cs140E, you can use debugging hardware to make powerful
binary analysis tools that normally take on the order of 1MLOC of code.

We will build the following:

  - [Trap-based Eraser]: Eraser is a famous race detection tool that
    uses binary-rewriting to instrumenting every load and store and
    flagging if a thread accessed the memory with inconsistent set of
    locks (its *lockset*).  Doing binary rewriting is hard.  By using
    debugging hardware and fast traps we can build a version in a few
    hundred lines of code: simply trap on every load and store and
    implement the lockset algorithm.
  
  - [Trap-based memory checking]: Valgrind dynamically rewrites binaries
    so that it can instruments every load and store.  Dynamically
    rewriting binary is harder in some ways than the static rewriting
    that Eraser does, which was already really hard.  You will use your
    traps to implement a much simpler method:

    - Mark all heap memory as unavailable.
    - In the trap handler, determine if the faulting address is in bounds.
    - If so: do the load or store and return.
    - If not: give an error.

    Given how fast our traps are, and how slow valgrind is, your approach
    may actually be faster.
   
    To get around the need for *referents* (that track which block of
    memory a pointer is supposed to point to) you'll use randomized
    allocation and multiple runs.

  - [Thorough conconcurrency checking]: we will use traps to build a simple,
    very thorough lock-free checking tool.  Given a lock free algorithm and
    two threads:

    - Mark its memory as inaccessible.
    - On each resulting fault, do the instruction but then context switch
      to another thread.
    - At the end, record the result.
    - Then rerun the thread code from a clean initial state without doing
      context switches.   For the type of code we run: Any difference
      is an error.

    To be really fancy, we can buffer up the stores that are done and
    then try all possible legal orders of them when the other thread
    does a load.  This checker should find a lot of bugs.

    To speed up checking we use state hashing to prune out redundant
    states: at each program counter value, hash all previous stores,
    if we get the same hash as before, we know we will compute the same
    results as a previous and can stop exploring.

  - [Possible: Volatile cross-checking] A common, nasty problem in
    embedded code is that programs code use raw pointers to manipulate
    device memory, but either the programmer does not use `volatile`
    correctly or the compiler has a bug.  We can detect such things with
    a simple hack:

    - We know that device references should remain the same no matter
      how the code is compiled.
    - So compile a piece of code multiple ways: with no optimization, `-O`,
    `-O2`, with fancier flags, etc.
    - Then run each different version, using ARM domain tricks to trace
      all address / values that are read and written.
    - Compare these: any difference signals a bug.

    This tool would have caught many errors we made when designing cs107e;
    some of them took days to track down.

   - There is a ton of other possible tools.  Might do more.

---------------------------------------------------------------------
##### Speed

Speed: in this class usually we waste a ton of cycles to buy simpicity
as a way to buy correctness.  ("The unavoidable price of correctness is
simplicity." --- Hoare.)

However, in this segment (probably 2 labs at least) we will change pace
and try to make your code as fast as possible.

Being able to speed things up is great; not always needed, but a fancy
tool if you can do it.

More importantly, speeding up our code --- which goes down to bare metal
(at the level of ASIDs, TLBs, different caches) and all the way up to
the OS and then application --- is a great way to deeply understand how
hardware and OS code actually work.

Time cuts across all boundaries so you cannot simply ignore ("abstract
away") pieces --- you have to understand each, and understand how
they interact.   Further, the process is ruthless with stupid: you
cannot fake it.  Either what you do cuts down time or it does not.
Optimization is a fun, limited example of "classical" science, where you
have to come up with hypotheses, act on these, and then do a completely
cut-and-dry experiment (run the code) that immediately cuts down the
bullshit or gives you some dopamine.

At a more conceptual level, making code as fast as possible requires
understanding its essence and cutting away the inessential.  The more
you understand, the faster you can go.  People that do not understand
much cannot write fast code --- everything is too much in shadow to find
the quick path.

With that said: "premature optimization is the root of all evil"
so we won't do this much.

  - Fast exceptions: Since our binary tools depend so much on exception
    handling we'll make it much more efficient.  Rip the code down to the
    bare minimum, enable icache, dcache, BTB, any other tuning you can.
    See how much faster you can make it compared to where we started.

  - Fast `libpi` code: you'll redo your statistical profiler from 140e and
    make it use backtraces to figure out where the code was called from.
    Using this you can speed up different pieces of `libpi` easily.

  - POSSIBLE: getting the maximum speed out of NRF24L01+.  You'll
    ping-pong packets between two devices and tune the code so that you
    get the maximum throughput possible. You can use the same tricks as
    for interrupts, but also play games with sending multiple packets
    in flight, using interrupts to help pipelining, etc.

  - POSSIBLE: Fast OS primitives.  If we build a simple OS it makes sense
    to tune its primitives.  We will run common micro-benchmarks on your
    laptop and see how much you can beat them by on the much-slower pi
    hardware using the much-faster code you wrote.    Example benchmarks:
    `fork()`, ping-pong across `pipe` implementations, cost of a
    protection fault, etc.  If you beat linux/macos by 50x I wouldn't
    be surprised.

---------------------------------------------------------------------
##### Timing-accurate code 

We couldn't afford to give everyone two r/pi's in 140e, but might be
able to squeek by in 240lx.  Which is great, b/c we can then build a
digital analyzer which is really useful.

  - [3-digital-analyzer](3-digital-analyzer/README.md): the pi is fast enough,
    and flexible enough that we can easily build a logic analyzer that
    (as far as I can tell) is faster --- and certainly more flexible ---
    then commercial logic analyzers that cost hundreds of dollars.
    You'll learn how to write consistently nanosecond accurate code.

    The digital analyzer can be viewed as a way to do `printf`
    for devices.  We'll use it to reverse engineer the SPI, I2C and
    UART protocols.  We'll also make it so you can upload code into
    it to do more interesting checks (e.g., that real time threads hit
    their schedules).

    A big challenge is getting the error / variance from one sample to
    another --- you'll use both memory protection tricks and interrupts
    to shrink the monitoring loop down far enough that it runs with an
    order of a few nanosecond error.

  - [ws2812b](ws2812b/README.md) you'll write nanosecond accurate
    code to control a WS2812B light string.  The WS2812B is fairly common,
    fairly cheap.  Each pixel has 8-bits of color (red-green-blue)
    and you can turn on individual lights in the array.

---------------------------------------------------------------------
##### Device code

You'll write some device drivers from scratch to get a better feel
for hardware.  Device driver code makes up 90+% of OS code, so it makes
sense to learn how to write this kind of code.   We'l likely do some
amount of building a fake-pi implementation to check this code.

  - [i2c-driver](i2c-driver/README.md): You'll use the broadcom
    document to write your own I2C implementation from scratch (no helper
    code) and delete ours.

    To help testing, you'll also write a driver for the popular ADS1115
    analog-to-digital converter (also from scratch).  Analog devices
    (that output a varying voltage) are typically the cheapest ones.
    Unfornately your r/pi cannot take their input directly.  We will
    use your I2C driver to read inputs from a microphone and control
    the light array with it.

  - [spi-driver]: SPI is a common, simple protocol.  You'll implement
    it and use it to control a digital-to-analogue converter and to
    control an OLED screen.

------------------------------------------------------------------
### Ideally: many other chips

We've written code on the r/pi A+, which is fine.  One thing I would love
to get out of this class is having the first set of 140E labs ported to
10+ other chips.  The tentative plan is to have a mid-term project where
you and some friends get a chip, get it up and running w/ a verson of our
bootloader and then writeup how to do so.  We'd then spend a monster week
of hacking having everyong write their code for every different chip.
The resulting portfolio should be extremely useful.


------------------------------------------------------------------
### POSSIBLE: RiscV

In cs340lx we spun up some hifive2 riscv boards and then wrote a simple
simulator that could run itself.  I later used this to make a symbolic
execution engine with a few hundred lines of code.  Using riscv is a
great method for doing binary analysis since its  a much simpler system
than ARM or valgrind intermediate representation.  We'll do several labs
building this.

------------------------------------------------------------------
### POSSIBLE: Protection domains

   - [Fast protection domains]: As you probably noticed, we have no
   protection against memory corruption.  As your system gets bigger,
   it becomes much much harder to track down which subsystem is causing
   everything to crash.  Going all the way to UNIX style processes
   is one attack on this problem, but we can use ARM do to a lighter
   weight method:

     1. Give each domain a unique domain number.
     2. Tag all memory associated with the domain with its domain number.
        (you may have to have additional domain numbers for pages accessible
        by more than one).
     3. Before you call into the domain, change the domain register so that
        domain has client privileges, but no other domain is accessible.
     4. At this point it can only touch its own memory.

   This method is much faster than switching between processes: we don't
   have to flush the TLB, caches, or page table.

   We'll do some tricks using linker scripts to make this relatively
   easy to setup.

   Subsystems corrupting and crashing systems is a serious problem in
   real OSes: 90% of their code is device drivers, and device driver
   code sucks.  A research system Nooks tried handle this problem by
   tracking the resources a device driver used, using compiler support
   to detect if the driver was about to corrupt memory --- at this point
   it would kill the driver and release its resources.  Given your OS
   we can build a version of this in a few hours.

------------------------------------------------------------------
### POSSIBLE: A tiny but complete OS

A big problem with cs140e is that we have a bunch of atomized pieces,
but didn't put everything into one working system.  We'll spend a few
labs getting a unix-ish simple system working that has virtual memory,
processes, inter-process communication, a simple file system etc.

   1. Build more of a small, somewhat "Turing complete" OS that can
      do interesting stuff: `sbrk` (and hence `malloc`), `pipe`,
      `waitpid`, a faster, more powerful version of `signal`, pre-emptive
      scheduling, some kind of real-time guarantees.

      One of the main points of this 140e/240lx/340lx is intentionally
      out of step with modern times: if you have a small code base you
      understand completely (e.g., b/c you wrote it) it's very very
      easy to do many things in a few hours that would warrant a full
      research paper and maybe more than a year of effort to build in
      a Linux or MacOS.  In many cases, you can build features that
      simply do not exist in current systems.    As one example of the
      former, in cs140e we built a `gprof` in about 60 lines of code.
      As examples of the latter: all of our memory tracing tools.

      As part of this genre I'm thinking in the next few labs we'll
      read two papers (one on memory tricks by Appel et al and one on
      "scheduler activations" by Bershad et al) and have you build
      those pieces.

      This approach will start to give you a more conceptual feel of key
      issues --- the papers are old, but the topics and tradeoffs are
      just as sharp today.  It will also show you currently actually
      have the tools to do research papers on your own --- proof by
      example since you will implement them!

      It also makes the value of a small code base clear: you should
      be able to bang these out in one medium lab each, despite modern
      OSes not being able to provide the functionality / efficiency
      they provide.  (Note: we will do the fast part soon.)

      Maybe your OS can't run many programs, but you can perform many
      actions that many (or all) cannot.

------------------------------------------------------------------
### POSSIBLE: A grab bag of things

  - have you write a simple static bug
    finding system based on "micro-grammars", a trick we came up with
    that does incomplete parsing of languages (so the parser is small
    --- a few hundred lines versus a million) but does it well enough
    that you can find many clean bugs with few false positives. It'd be
    fantastic if you guys could write your own system and go out and find
    a bunch of real bugs in the large systems you'll be dealing with.
    The main limit is me finding a few days to build this out and try
    to break it down in a way that doesn't require compiler background
    (this is feasible, I am just badly managing time).

  - Make supe tiny processes.  We can't run many programs, but we can
    make our processes orders of magnitude smaller than in other OSes.
    E.g., if we pin TLB entries we don't even need a page table!  If you
    then cut down on the size of the env structure and use small pages
    (either 4k or 1k) rather than 1MB you can make absolutely tiny
    processes.  I think we could run tens of thousands on the chip, which
    would be unheard of for any other OS on an equivalant sized system.
    This would be cool and teach a bunch (it's similar to the optimization
    we would do but would be for space vs time.)

  - Make a simple distributed system.  One use case is doing crypto
    mining on a linked set of r/pi chips.

  - As a way to help testing (among other things): do a real writeable
    file system so that we can save the binaries to run, the output,
    etc and use scripting (via FUSE) to easily interact with your pi
    through your normal shell.  We could also use the RF chip to make
    this distributed.

  - More speculatively: do very sleazy symbolic execution  to generate
    inputs to run code on (and, e.g., find differences in today's code
    versus yesterday's) by inferring what each machine instruction does by
    running it in its own address space, seeing how the registers change,
    how memory is read and written, and then matching this against known
    micro-ops (or compositions of them).  You can then use this to run
    the binary to get constraints and then use a constraint solver to
    solve them and rerun the result on the code.  If we can do this it
    will be a very powerful trick, since its simple enough to work in
    real life (unlike the current symbolic systems).

