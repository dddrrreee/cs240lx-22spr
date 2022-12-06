#include "rpi.h"
#include "rpi-interrupts.h"
#include "backtrace.h"
#include "uart-int.h"

void d(){printk("d\n"); delay_ms(200);}
void c(){printk("c\n"); delay_ms(200); d();}
void b(){printk("b\n"); delay_ms(200); c();}
void a(){printk("a\n"); delay_ms(200); b();}


void interrupt_vector(unsigned pc){

    if (!is_int_pending()){
        panic("Interrupt Type Unhandled!\n");
    }

    // Read from the UART FIFO and decide 
    // whether to print the backtrace.
    unimplemented();
}

void notmain(void) {
    // set up interrupt table.
    int_init();
    uart_int_init_off();
    printk("switching on interrupts\n");
    // flush uart before enabling interrupting.
    uart_flush_tx();
    // enable rx interrupts.
    uart_rx_int_init();
    // enable aux interrupts for mini-uart.
    uart_int_init_on();
    // enable system wide interrupts.
    system_enable_interrupts();

    while(1){
        // a() -> b() -> c() -> d()
        // Press the key that triggers backtrace to 
        // see backtrace in action!
        a();
    }
}
