#pragma once
#include <stdint.h>

#define APPEND_GLOBAL(NAME) (*({ \
    NAME = realloc(NAME, (++N_##NAME) * sizeof(NAME[0])); \
    NAME + N_##NAME - 1; \
}))

#define APPEND_FIELD(OBJ, FIELD) (*({ \
    (OBJ).FIELD = realloc((OBJ).FIELD, (++(OBJ).n_##FIELD) * sizeof((OBJ).FIELD[0])); \
    (OBJ).FIELD + ((OBJ).n_##FIELD - 1); \
}))

// Create a fresh array and return a handle to it
int new_array();
// Return an updated array where the only difference is bv_key's value is now
// bv_value
int array_store(int old_array, int bv_key, int bv_value);
// Return the value of bv_key in this array
int array_get(int array, int bv_key, int out_width);
// Create a fresh bitvector of the given width and return a handle to it
int new_bv(int width);
// Create a *constant* bitvector of the given width with the given value and
// return a handle to it
int const_bv(int64_t value, int width);
// Create a new literal/variable that is true iff the bitvectors represented by
// handles bv_1 and bv_2 are equal.
int bv_eq(int bv_1, int bv_2);
// Return a handle to a fresh bitvector with value equal to the sum of that of
// bv_1, bv_2.
int bv_add(int bv_1, int bv_2);

// Add a clause/assertion to the underlying SAT instance. E.g., to assert that
// bv1 and bv2 are handles to distinct bitvectors, use clause(-bv_eq(bv1, bv2))
void clause_arr(int *literals);
#define clause(...) clause_arr((int[]){__VA_ARGS__, 0})

// Try to find a solution to the current set of constraints. Should only be
// called once (per process). Returns 0 for unsat, 1 for sat. In the latter
// case, the satisfying model can be queried with get_solution.
int solve();
// Query the model that satisfies the current constraints. Can only be called
// once solve() is called and returns 1.
int64_t get_solution(int bv, int as_signed);
