# Hints
This lab ended up being a bit longer than I expected. Here I've compiled some
hints you may find helpful. There are multiple ways to encode these things, and
the encodings here are not the best: feel free to experiment!

### Encoding bv_eq
Suppose we have the following code:
```
// Create bitvectors x, y with width 2
int x = new_bv(2),
    y = new_bv(2);
// Assert that they're disequal
clause(-bv_eq(x, y));
```
The calls to `new_bv` allocate fresh "SAT-land" variables for each bit of `x`
and `y`. Suppose `x` has SAT-land bits `1, 2` and `y` has `3, 4`. The goal of
`bv_eq` is to create a SAT-land literal that is true if and only if `x` and `y`
have the same bits. We can encode this like:
```
5 <=> (1 <=> 3)
6 <=> (2 <=> 4)
7 <=> 5 and 6
```

Now how to encode `5 <=> (1 <=> 3)`? To do this we need to encode:
```
5 => (1 <=> 3)
(1 <=> 3) => 5
```
It will turn out that using a different encoding of `(1 <=> 3)` for the first
vs. second will be helpful to simplify this.

Notice that `1 <=> 3` can be encoded as either:
- `(1 => 3) and (3 => 1)`, i.e., `{-1, 3} and {-3, 1}` OR
- `(1 and 3) or (-1 and -3)`.

So let's encode `5 => (1 <=> 3)` as
```
5 => {-1, 3} and {-3, 1}
is the same as:
5 => {-1, 3}
5 => {-3, 1}
is the same as:
{-5, -1, 3}
{-5, -3, 1}
```
And let's encode `(1 <=> 3) => 5` as
```
(1 and 3) or (-1 and -3) => 5
is the same as:
(1 and 3) => 5
(-1 and -3) => 5
is the same as:
{-1, -3, 5}
{1, 3, 5}
```

Now we're done with the first thing we wanted to encode. Doing the same trick
lets you encode `6 <=> (2 <=> 4)`. All that's left is `7 <=> 5 and 6`. This is:
```
7 => 5 and 6
5 and 6 => 7
is the same as
7 => 5
7 => 6
5 and 6 => 7
is the same as
{-7, 5}
{-7, 6}
{-5, -6, 7}
```
For the very last clause, you'll want to use the `clause_arr` function rather
than `clause` (since it can be arbitrarily large depending on the variable's
bitwidth).

And that's `bv_eq`!

### Bitvector Addition
Think lookup table for a ripple-carry adder. Create some temporary SAT-land
variables for the carry bits, fix the first carry bit to false, and then add
clauses that compute output & carry bits from the input & prior carry bits.
This should use 16 clauses per loop iteration, looking something like:
```
1 + 1 + 1 = 11b
(x[i] and y[i] and c[i]) => z[i]
(x[i] and y[i] and c[i]) => c[i]

1 + 1 + 0 = 10b
(x[i] and y[i] and -c[i]) => -z[i]
(x[i] and y[i] and -c[i]) => c[i]

Etc...
```
Once you have the first row written out it's just a bunch of copy/paste.

### Encoding Arrays
The one big thing to know about our array encoding is that arrays don't show up
in the SAT encoding until the very, very end (`array_axioms` called by
`solve`). Until then, the array operations are basically just tracking metadata
about bitvectors, e.g., "bitvector 10 is equal to the value at index given by
bitvector 6 in array 2." Once we are asked to solve the constraints, though, we
have to somehow communicate this array information to the SAT solver. This is
done with the "read-over-write" axioms. To explain these, suppose you have code
like this (ignoring bitvector widths):

```
int k1 = new_bv(), k2 = new_bv(), k3 = new_bv(), k4 = new_bv(),
    v1 = new_bv(), v2 = new_bv();
int a1 = new_array();
int a2 = array_set(a1, k1, v1);
int x1 = array_get(a2, k2);
int a3 = array_set(a2, k3, v2);
int x2 = array_get(a3, k4);
```
(Note: `array_set` is not "in-place;" think of it as copying the source array,
changing one index in the copy, and then returning the copy.)

If you encode these bitvectors without adding any array constraints (basically,
no clauses for this example), what could go wrong? Well, the SAT solver could
give you back anything. E.g., it could give a solution where `k1 = k2` but
`x1 != v1`. This is clearly wrong: if you set key `k1` to value `v1`, and then
check the value at location `k2 = k1`, you should get back something equal to
the value you stored! We encode that for this read like so:
```
(k2 = k1) => (k1 = v1)
```
(Note in `array_axioms` you *can* call `bv_eq`!)

We need to do the same for the read of `k4` on the last line. Clearly, if we
read at `k3` (where we just wrote) we need to return what we stored:
```
(k4 = k3) => (x2 = v2)
```
On the other hand, if we did not read what we just assigned to but we did read
what we assigned to earlier, we need to get back what we stored earlier:
```
((k4 != k3) and (k4 = k1)) => (x2 = v1)
```

There's one last think that can go wrong. Suppose `k4 != k3`, `k4 != k1`,
`k2 != k1`, *but* `k4 = k2`. In other words, we never read anything we
explicitly set, but we read the same key twice. With just the clauses we added
so far, the SAT solver could still give us a solution where `x1 != x2` even
though `k4 = k2`. To rule this out, we need to add a clause:
```
((k4 != k3) and (k4 = k2)) => (x2 = x1)
```

And that's arrays!
