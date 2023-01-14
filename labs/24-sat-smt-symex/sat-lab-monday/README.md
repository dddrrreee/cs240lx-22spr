# Chaff/SAT Solver Lab by Matthew

In this lab we'll be reproducing the Chaff paper. In particular, we'll focus on
the following two claims:
1. In a normal SAT solver, around 90% of the time is spent in boolean
   constraint propagation.
2. Using watched literals instead of a counter can achieve a ~5--10x speedup
   over the basic DP SAT solver algorithm.

There isn't a lot of code to write in the first place. My version of part 1
ends up with ~150 loc (~25 lines over the starter code) and my version of part
2 end sup with ~190 loc (~30 lines over the start code). But there are a fairly
large number of interacting data structures (mostly simple heap arrays) so it
may longer than expected. If you'd like more of a challenge, try starting all
the functions from scratch or try implementing some of the remaining tricks in
the "What's Next?" section below!

## Part 1: Implement & profile the basic DP algorithm
#### Implementation
Implement the `assert(!"Implement me!");` locations in `basic.c`. The comments
should instruct you on what to do. I would highly recommend first reading the
comments in the data structures section.

The program accepts DIMACS input on `stdin`. To run this on a file
`foo.dimacs`, use:
```
$ cat foo.dimacs | ./basic
```

#### Debugging
Note that SAT solvers are surprisingly difficult to implement correctly.
If you make a mistake it's liable to keep getting the right answer on 90% of
inputs, or all inputs that don't repeat literals in a clause, or ...

To counteract this, we're going to use the same cross checking approach we've
used forever in this class: observe some part of the execution that is supposed
to be invariant to our optimizations, and compare multiple implementations with
respect to that observation.

The obvious choice for observation is sat vs. unsat, but that's too coarse.
Instead, we'll print out the sequence of decisions and compare those. This is
already done for you, you don't need to modify anything (see the call to
`xprintf` in `decide()`).

To cross-check your basic implementation against mine, try:
```
./cross_check.sh basic_vs_matthew
```

To help with debugging, you can uncomment the `-fsanitize=address` line in the
`Makefile` and `make -B` to build with sanitizers. But make sure to re-comment
& re-build before profiling!

#### Profiling
The Chaff paper motivates their watched literals hack by the following
observation: "In practice, for most SAT problems, a major potion (greater than
90% in most cases) of the solvers' run time is spent in the BCP process.
Therefore, an efficient BCP engine is key to any SAT solver."

Let's see if we can reproduce this claim. Install `perf` and run the following:
```
$ make -B
$ cat ../test_inputs/p8.dimacs | sudo perf record ./basic
$ sudo perf report
```
You should see in the resulting report that BCP takes about 85% of the time,
pretty close to their claim!

Now that we're convinced of the problem, let's try out their solution.

## Part 2: Implement the watched literals optimization
Now we'll be implementing the watched literals optimization. Whereas before
`LIT_TO_CLAUSES[lit]` was an array of all clauses containing the literal `lit`,
now `LIT_TO_CLAUSES[lit]` will be a linked list of only those clauses that are
*watching* the literal `lit`. As in the paper, every clause will be watching
exactly two literals at a time. We're using a linked list to make it updating
which literal is being watched constant-time.

Your first task is to implement `init_watcher`, which is called by `main()` to
initialize `LIT_TO_CLAUSES` and the watched literals on the clause struct
(replacing `n_zeros`).

We also have a new `BCP_LIST` data structure. The idea is that `set_literal`
will now immediately identify implied literals, and queue them up in `BCP_LIST`
to be assigned during the `bcp()` phase. You'll notice that `bcp()` has gotten
a _lot_ simpler --- it just does what `set_literal` queued up for it!

Your second task is to implement `queue_bcp` and `dequeue_bcp`, which are
exactly what they sound like (except, er, it's probably faster to just do a
stack not a queue! doesn't matter for correctness, but I like the sound of the
word "queue" better...).

The real big task is now implementing `set_literal`. This is where almost all
the serious work of the watched-literals-optimized version is done. This one
function is basically going to do the work of both `set_literal` and `bcp` from
`basic.c`.

...but, payoff from that is we no longer have to do _anything_ non-trivial in
`unset_latest_assignment`! We literally get to just delete all that code!

You should be able to copy/paste your `decide()` from `basic.c`.
`resolveConflict()` can also be _almost_ the same --- just remember now you
also have to clear out the pending BCP list!

After that, you should be good to go.

To cross-check your basic implementation against your optimized implementation,
try:
```
./cross_check.sh basic_vs_fast
```
Note that this cross-checks on a fairly hefty input, and the xcheck logs can be
pretty large (tens of megabytes). So it may take significantly longer to run in
xcheck mode than directly.

## What's Left?
If you try comparing the time to solve, say, `p9.dimacs` with your `./fast` and
`minisat` you'll find something a bit wacky: our `./fast` is actually faster
than `minisat`!

Why is this? I don't _think_ it's because of a bug in our solver; if you
compare the number of decisions made (in `xcheck` mode) to the number reported
by `minisat`, it's pretty comparable.

I think what's happening is much simpler: the pigeonhole formulae (like
`p9.dimacs`) are sort of the "worst possible thing" for SAT solvers. Minisat
has a bunch of extra tricks that try to speed things up in the "happy case,"
but none of them are useful for pigeonhole formulae. Instead, they just take up
time without actually improving the search at all. This is a delicate balance,
where some tricks will speed up problems X, Y, Z but slow down A, B, C. More of
an art than a science, etc.

You can confirm this by finding inputs, like `aim-100.dimacs`, that complete
super fast on minisat but are terrible in our solver.

What are those extra tricks? A few things:
1. Conflict-driven clause learning (CDCL). Right now, if we get a conflict we
   just backtrack to the latest decision not "tried both ways." In essence, all
   we learn is that that exact sequence of decisions cannot work. There are
   techniques that can spend a little extra time analyzing the conflict in
   detail and figure out a new constraint that rules out not just that exact
   sequence of decisions, but perhaps other ones as well. More concretely,
   imagine some formula where you can prove there is no solution with `1` and
   `3` both set to false. The solver may end up in a decision stack like
   `[-1,-2,-3]` and then get a conflict. Our current implementation would
   backtrack repeatedly, trying `[-1,-2,3]`, `[-1,2,-3]`, `[-1,2,3]`, etc.
   Meanwhile, a CDCL implementation would _analyze the conflict_ in detail, and
   (if we're lucky) determine that `-1`, `-3` is the "core" of the conflict. It
   then produces a new clause that rules this out, i.e., `-1 3` (or `-3 1`).
   With this new clause added to the formula, the solver will never even
   consider assignment `[-1,2,-3]`, because the `-1` will BCP in `-3`. This
   lets us prune our search space more aggressively than just standard
   backtracking. A nice interactive explanation of this process is given here:
   https://cse442-17f.github.io/Conflict-Driven-Clause-Learning/
2. In a basic CDCL solver, every time we reach a conflict we actually add a new
   clause to the clause list. This is nice because new clauses help prune our
   search space (in other words, we get more opportunities for BCP instead of
   having to do decide). But it can get messy if we have _way_ too many clauses
   (most of them are probably not that useful any more, if we've already
   "passed out of that area of the search space"). _Clause deletion heuristics_
   tell us which clauses are probably OK to delete/clean out. See
   https://webdocs.cs.ualberta.ca/~mdsolimu/CP19_GluVar_Final.pdf for a
   discussion of one approach.
3. Decision heuristics. In our `decide()`, we always just take the
   numerically-next unassigned variable and try assigning it to false. This is
   not a great idea. If a 100 clauses have that variable in positive form and
   only one has it in negative form, it's probably a good idea to try and set
   it to true first, as that "gets out of the way" the most clauses at once.
   The Chaff paper describes a heuristic called VSIDS that they claim is fast
   and pretty good. I bet you could probably implement it with another 100
   lines of code or so.
4. All of the above modifications make the solver very, very stateful. During
   the solver run the solver is learning things about the formula that it
   didn't know when it started (e.g., VSIDS basically approximates which
   variables are more likely to be true vs. false in a solution). Intuitively,
   it's possible that you made some bad decisions in the beginning of the solve
   that got you stuck in an unproductive part of the search space. But maybe if
   you had known all that information early on in the solve, you would have
   made better decisions that could have gotten you more quickly into a better
   part of the search space. The trick to take advantage of this is to just
   randomly restart the solver, but keep some of the information learned during
   this attempt around (e.g., the VSIDS counts).
5. I don't think many common solvers actually do this, but another cool one is
   _symmetry breaking_. The basic idea is that a lot of problems are symmetric;
   think about a formula involving something like `x^2 + y^2`. Any solution
   `(x,y)` can be swapped to `(y,x)` to form an equally-good solution. In
   theory, then, we can cut down the set of possible solutions we have to think
   about by only considering those where `x <= y`. This is called "symmetry
   breaking."
