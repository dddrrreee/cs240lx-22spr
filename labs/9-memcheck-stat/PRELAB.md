Memory corruption bugs suck.  While some people use Rust, we're going
to try to make C less bad by attempting to check every load and store
for safety.

One weak way to find memory bugs in C is to use a debugging `malloc` that
embeds each allocated block between two "redzones" (one immediately above
it, one immediately below it) set to known values and then periodically
check if any redzone has been corrupted.  This method is better than
nothing, but can miss many errors.  (E.g., it only checks for corruption
when the programmer manually says to, and it misses when you  add an
illegal offset to a pointe that happens to take it from one allocated
block to another one).

Another method is to use a full-on binary analysis tool such as Purify
or Valgrind.  These rewrite your binary and insert checks at every
single load and store.  Because it's in-practice impossible to perfectly
distinguish code and data, Valgrind and related tools do their rewriting
at runtime (similar to how VMware used to) using the heuristic: if you
tried to run something, it is an instruction.  This approach lets them
check your program thoroughly, but at a cost of astonishing complexity.
Valgrind is easily over 100,000 lines of very tricky code.  Both because
of this complexity, inflexibility and slowness, you cannot use them
to check kernel code or embedded, the two domains that could benefit
the most.

In this lab we're going to do a simple hack that, at least as far as
I know, no one else has ever done, which will make it super simple to
instrument loads and stores and look for memory corruption:
  1. Key idea: Use interrupts to discover code.  Any PC we interrupt is, by 
     definition, an instruction.
  2. Disassemble this instruction to see if it is a load or store.
  3. If so, replace it with a fixed jump to a trampoline location we will
     generate at runtime that does
     an in-bounds memory check and then jumps back.
  4. By using a trampoline, we do not dialate the executable:
     as a result, all instructions that use code addresses will still work.

The longer we run, the more memory operations we detect and can check.

So, prelab:
  1. Use your lab 2 reverse engineering code to derive how to decode
     loads, stores, pushes, pops and jumps so that we can decode
     these instructions thursday.
  2. Read the Purify paper in `docs` --- it's a very simple paper
     that had a lot of impact (and when the company sold, Reed Hastings
     used the money from the sale to start NetFlix).
  3. Optional: There are a few other useful papers in `docs` if you want more 
     context for the general approach of binary rewriting.  The ideas
     in the Atom paper are great to know.
