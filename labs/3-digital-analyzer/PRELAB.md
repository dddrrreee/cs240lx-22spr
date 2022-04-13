### PRELAB

Physical setup:

  1. Setup your second pi so that it can bootload, just like your first
     and  that you can attach both pi's at once to your laptop.
  2. Make sure you have female-to-female wires to hook both pi's together.


Useful documents, all in the `docs` directory, ordered
by increasing detail:
   1. [Quick reference](../../docs/arm-asm-quick-ref.pdf).
   2. [Instruction set summary](../../docs/armv6-inst.pdf).
   3. [Instruction set full list: `../../docs/armv6-inst-full.pdf).
   4. [The full manual for the ARMV6 family](../../armv6.annot.pdf).
   5. [The full manual for our chip] (../../docs/arm1176.pdf).

Assembly background:

  1. Make sure you feel ok disassembling the `.bin` executables and
     looking at the generated machine code (currently our `Makefile`s
     automatically disassemble the `.bin` into a `.list` file).

     You'll be doing this a bunch here to make you system more accurate.

  2. Most of today's lab is getting `gcc` to emit good code. Sometimes
     it's worth tricking `gcc` into not doing
     something dumb, occasionally it is easier to just
     use inline assembly.  [This writeup of ARM inline
     assembly](http://www.ethernut.de/en/documents/arm-inline-asm.html)
     seemed reasonable; there may be better ones, so don't be shy about
     searching around and reporting back.

     [Official gcc inline assembly](http://199.104.150.52/computers/gcc_inline.html).

Do these measurements:

  1. We will be measuring cycles, so (1) figure out how many cycles per
     second our A+ pi's can execute and (2) how many nanoseconds per
     cycle.  Write some code to verify this using `timer_get_usec()` and 
     `cycles_cnt_read()`.

  2. Measure the cycle cost of various operations such as: (1) the cost
     of a GPIO load, (2) the cost of a GPIO store, and (3) the cost of
     getting the current cycle.
