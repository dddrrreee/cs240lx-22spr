#include <stdio.h>
#include <stdlib.h>

void test(void){
    u_int8_t * p = malloc(8);
    p[1] = 8;
    u_int8_t num = p[1];
    free(p);
    p =  malloc(8);
    free(p);
    printf("Should have run without any errors...\n");
}

int main() {
    test();
    return 0;
}
