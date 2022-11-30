#include <stdio.h>
#include <assert.h>
#include "smt.h"

// Test all bitvector functions, including add. Uses the SMT solver to compute
// subtraction, i.e., solve for x such that 11 + x = 23.
int main() {
    printf("Testing bitvectors with addition...\n");

    int bv_const_11 = const_bv(11, 10),
        bv_const_23 = const_bv(23, 10);
    int bv_x = new_bv(10);

    int bv_11_plus_x = bv_add(bv_const_11, bv_x);

    // assert 11 + x == 23
    clause(bv_eq(bv_11_plus_x, bv_const_23));

    // Should be sat...
    assert(solve());

    // And 11 = 11, 23 = 23, and x = 12
    assert(get_solution(bv_const_11, 0) == 11);
    assert(get_solution(bv_const_23, 0) == 23);
    assert(get_solution(bv_x, 0)        == 12);

    printf("Passed!\n");
    return 0;
}
