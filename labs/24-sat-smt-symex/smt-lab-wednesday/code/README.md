# SMT + SymEx Lab
The goal of this lab is to build a working symbolic execution engine off of our
SAT solver.

### Symbolic Interpreter
We'll work top-down, building the symbolic interpreter against my SMT solver
binary so that we can start running test cases ASAP. Then you'll swap your SMT
solver for mine once you have that working.

We'll work on a simple little assembly-like IR. This IR has:
- Arbitrarily many registers
- A word size determined by the `WORD_SIZE` global in `main.c`
- A heap memory indexed by registers
- A branching instruction that jumps to a relative offset if two registers have
  the same value
- A "failure" instruction that indicates any path reaching this is a bug

The idea is to basically implement a little interpreter for this language
except:
1. Instead of using concrete integers for register/memory values, use
   bitvectors and array operations as exposed by the SMT solver library.
2. When a branch statement is reached, simply fork the interpreter. In one
   process, assert the branch is taken and proceed. In the other, assert it's
   not and proceed.
3. When a failure statement is reached, try to solve the current path
   constraints. If a solution is found, that represents an input that can reach
   this failure location, i.e., a bug. If we visit all possible paths and none
   can reach fail, we've proven the absence of such bugs.

The trickiest part is actually printing out the solution at the end. We need to
track the first time we use a register, as well as the first time we read a
certain value in memory (since these represent possible inputs to our program).

All you need to do is implement the instructions symbolically by calling out to
the methods in `smt.h`. Use `get_register` and `set_register` to get/set
bitvectors representing register values; these methods will automatically
update IN_VARS for register operations, but you'll need to do it yourself for
memory operations.

### SMT Solver
The core of our symex engine will be an SMT solver. An SMT engine is a nice
frontend to a SAT solver, which allows you to construct constraints involving
arrays and bitvectors (finite bitwidth integers, like in C).

For simplicity, we'll be building an eager SMT solver, also known as bit
banging. This is the same fundamental approach used by the STP solver:
https://github.com/stp/stp

The idea is to "compile" the higher-level constraints into a big SAT problem,
then use our SAT solver to solve this problem.  This approach is nice because
it makes a clean separation between the SMT solver and SAT solver; you can test
against your SMT solver against any SAT solver to better localize bugs, and we
don't have to change our SAT solver at all.

When you create a bitvector of size N, it actually creates N new bits in the
SAT problem representing the N bits of the bitvector. The method `bv_eq(b1,b2)`
creates a new SAT variable and spits out clauses ensuring that variable is true
iff all of the bits in b1 and b2 are identical. Similar for `bv_add(b1,b2)` ---
we essentially spit out a ripple-carry adder in the underlying SAT constraints.

Arrays are a bit more complicated. When the user requests `arr[x]`, we give
back a fresh bitvector and record separately that it's supposed to be `arr[x]`.
Then, before spitting out the final DIMACS file, we look at every earlier call
to `arr[y]` and add assertions that `arr[x] = arr[y]` if `x = y` (being careful
to handle cases where we overwrite `arr[x]`!).

Then, we print the constraints to a DIMACS file, run our SAT solver, and read
back in the results.

Some tips/reminders for the SAT encoding:
- `x <=> y` is the same as `x => y` and `y => x`
- `x => z` is equivalent to the clause `{-x, z}`
- `x and y => z` is `{-x, -y, z}`
- `x or y => z` is `x => z` and `y => z`
- `x => y and z` is `x => y` and `x => z`
- `x => y or z` is `{-x, y, z}`

### Extensions
- Extend the symex IR to support more operations
- Write a compiler (or interpreter!) from a higher-level language (some useful
  subset of C?) to the symex IR
- Modern SMT solvers often use a *lazy* encoding instead of eager/bitbanging.
  The idea is to only give the SAT solver a subset of the clauses; if it says
  "unsat" with that subset, you're already done. Otherwise, check if its
  solution also satisfies the remaining clauses. If not, give it a few more
  clauses until those clauses rule out the current attempted solution. Repeat
  this until you either get unsat or find a solution that works for all the
  clauses. This is often faster in part because it lets you check the remaining
  clauses however you want, without having to somehow encode them into CNF.
  Also, you may not need all the clauses to prove unsat (or even SAT), so just
  adding them as-needed is a good idea.