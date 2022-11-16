#include <stdio.h>
#include <stdlib.h>

void test(int arg){
    printf("Executing test with argument %d\n", arg);
}

int main(){
    printf("Executing main.\n");
    test(42);
    test(11);
    return 0;
}
