# SAT/Symex+SMT Labs Prelab
By Wednesday, we will make a symbolic execution (symex) engine for a simple
little "assembly language." Think of a symex engine as a tool that tries to
generate inputs to your program that cause it to execute a certain line. For
example, in this program:
```
load 0 0        ; r0 = memory[r0]
immediate 1 10  ; r1 = 10
branch_eq 0 1 2 ; if r0 == r1 goto l1 else goto l2
l1: fail
l2: exit
```
The symex engine will attempt to find initial registers & memory values that
cause the `fail` instruction to be reached. In this case, it will say setting
`r0 = 0`, `memory[0] = 10` does the trick.

Symex engines are built on SMT solvers which are built on SAT solvers.
Roughly, SAT solvers are symex for when you have no branching, no memory, and
finitely-many boolean registers. The only operations allowed are "and," "or,"
and "not." SMT solvers add support for int (non-boolean) values and memory.
Symex engines add support for branching, loops, and some assembly/code syntax.

### Monday: SAT Solver Prelab
SAT solvers solve _boolean constraints_. For example, you can say "find an
assignment for $x$, $y$, $z$ such that: (i) $z$ is true, (ii) if $x$ and $y$
are true then $z$ is false, and (iii) either $x$ or $y$ is true."

In order to keep SAT solvers as simple and fast as possible, the input language
describing constraints for most SAT solvers is pretty restrictive. Here's an
example:
```
c Amazing comment!!
p cnf 3 3
3 0
-1 -2 -3 0
1 2 0
```
- Initial lines can start with `c` for `c`omments.
- The `p` (for `p`roblem?) line tells us how many variables and how many
  clauses (remaining lines).
- The constraints are given in AND of OR form; each line represents an OR and
  every line has to be satisfied (AND).
- Every line ends in `0` (variables are numbered starting from `1`).
- `n` means that variable should be true, `-n` means false.
- The basic idea is to assign each variable true or false so that every line
  "predicts" at least one variable's true/falseness correctly.
In the above example, line `3 0` means "variable 3 should be true," the next
line means "either variable 1 is false, or variable 2 is false, or variable 3
is false," and the last line means "either variable 1 is true or variable 2 is
true." If you think about it, those constraints are exactly the same as (i),
(ii), (iii) from earlier.

You can plug this in to minsat (or the [web
version](http://logicrunch.it.uu.se:4096/~wv/minisat/)) to get a solution:
```
SATISFIABLE

-1 2 3 0
```
This means there is a solution, namely, variable 1 false, variable 2 true, and
variable 3 true.

We'll be writing our own version of minisat, including a few hacks to speed it
up. We'll be basing our version on the SAT solver called Chaff, which you
should read about
[here](https://www.princeton.edu/~chaff/publication/DAC2001v56.pdf)
(specifically sections 1 and 2!).

### SMT/Symex Prelab
TODO
