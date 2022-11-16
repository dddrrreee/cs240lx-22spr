#include <stdio.h>


#define rtlib_output(args...) do {      \
    printf("RTLIB: ");                  \
    printf(args);                       \
} while(0)

void print_hello(){
    rtlib_output("Hello from instrumented function!\n");
}

void print_with_arg(int arg){
    rtlib_output("Argument Passed = %d\n", arg);
}


