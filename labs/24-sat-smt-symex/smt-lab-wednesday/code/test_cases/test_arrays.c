#include <stdio.h>
#include <assert.h>
#include "smt.h"

// Tests new_array, array_store, array_get
int main() {
    printf("Testing arrays...\n");

    int bv_const_10 = const_bv(10, 10),
        bv_const_0 = const_bv(0, 10),
        bv_x = new_bv(10),
        bv_y = new_bv(10);

    int arr0 = new_array();
    // Store x into 10
    int arr1 = array_store(arr0, bv_const_10, bv_x);
    // Store y into x
    int arr2 = array_store(arr1, bv_x, bv_y);
    // Store x into arr2[10]
    int arr3 = array_store(arr2, array_get(arr2, bv_const_10, 10), bv_x);

    // Assert x, y are both non-zero and both distinct
    clause(-bv_eq(bv_x, bv_const_0));
    clause(-bv_eq(bv_y, bv_const_0));
    clause(-bv_eq(bv_x, bv_y));

    // We're going to check the following facts:
    // (1) arr1[10] == x
    // (2) arr2[x] == y
    // (3) if x == 10 then arr2[10] == y
    //     else (arr2[10] == x && arr2[x] == y)
    // (4) if x == 10 then (arr3[y] == x && arr3[x] == arr2[x]),
    //     else arr3[x] == x
    int arr1_10 = array_get(arr1, bv_const_10, 10),
        arr2_x  = array_get(arr2, bv_x, 10),
        arr2_10 = array_get(arr2, bv_const_10, 10),
        arr3_y  = array_get(arr3, bv_y, 10),
        arr3_x  = array_get(arr3, bv_x, 10);

    // Should be sat...
    assert(solve());

    assert(get_solution(arr1_10, 0) == get_solution(bv_x, 0));
    assert(get_solution(arr2_x, 0) == get_solution(bv_y, 0));
    if (get_solution(bv_x, 0) == 10) {
        assert(get_solution(arr2_10, 0) == get_solution(bv_y, 0));
    } else {
        assert(get_solution(arr2_10, 0) == get_solution(bv_x, 0));
        assert(get_solution(arr2_x, 0) == get_solution(bv_y, 0));
    }
    if (get_solution(bv_x, 0) == 10) {
        assert(get_solution(arr3_y, 0) == get_solution(bv_x, 0));
        assert(get_solution(arr3_x, 0) == get_solution(arr2_x, 0));
    } else {
        assert(get_solution(arr3_x, 0) == get_solution(bv_x, 0));
    }

    // And the inequality & const clauses we added are important too...
    assert(get_solution(bv_const_10, 0) == 10);
    assert(get_solution(bv_const_0, 0)  == 0);
    assert(get_solution(bv_x, 0) != get_solution(bv_y, 0));
    assert(get_solution(bv_x, 0));
    assert(get_solution(bv_y, 0));

    printf("Passed!\n");
    return 0;
}
