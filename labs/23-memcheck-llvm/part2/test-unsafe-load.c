#include <stdio.h>
#include <stdlib.h>

void test(void){
    u_int8_t * p = malloc(8);
    printf("Trying unsafe load now...\n");
    u_int8_t num = p[10];
    free(p);
}

int main() {
    test();
    return 0;
}
