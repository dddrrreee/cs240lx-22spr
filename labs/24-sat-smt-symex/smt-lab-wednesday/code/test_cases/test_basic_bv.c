#include <stdio.h>
#include <assert.h>
#include "smt.h"

// Tests new_bv, const_bv, bv_eq, solve, and get_solution.
int main() {
    printf("Testing basic bitvectors...\n");

    int bv_const_10 = const_bv(10, 10);
    int bv_x = new_bv(10),
        bv_y = new_bv(10);

    // assert x == const_10
    clause(bv_eq(bv_x, bv_const_10));
    // assert x == y
    clause(bv_eq(bv_x, bv_y));

    // Should be sat...
    assert(solve());

    // And everything should be 10!
    assert(get_solution(bv_const_10, 0) == 10);
    assert(get_solution(bv_x, 0)        == 10);
    assert(get_solution(bv_y, 0)        == 10);

    printf("Passed!\n");
    return 0;
}
