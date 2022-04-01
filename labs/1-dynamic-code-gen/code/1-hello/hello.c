#include "rpi.h"
#include "../unix-side/armv6-insts.h"

void hello(void) { 
    printk("hello world\n");
}

// i would call this instead of printk if you have problems getting
// ldr figured out.
void foo(int x) { 
    printk("foo was passed %d\n", x);
}

void notmain(void) {
    // generate a dynamic call to hello world.
    // 1. you'll have to save/restore registers
    // 2. load the string address [likely using ldr]
    // 3. call printk
    static uint32_t code[16];
    unsigned n = 0;

    unimplemented();

    printk("emitted code:\n");
    for(int i = 0; i < n; i++) 
        printk("code[%d]=0x%x\n", i, code[i]);

    void (*fp)(void) = (typeof(fp))code;
    printk("about to call: %x\n", fp);
    printk("--------------------------------------\n");
    fp();
    printk("--------------------------------------\n");
    printk("success!\n");
    clean_reboot();
}
