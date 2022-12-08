#ifndef __GPIO_H__
#define __GPIO_H__

#define RW_OFFSET 0x0000
#define XOR_OFFSET 0x1000
// Think of this as an or
#define SET_OFFSET 0x2000
// This would be more of an and
#define CLR_OFFSET 0x3000

#include <stdint.h>
#define IO_BANK_BASE 0x40014000 
enum {
   // set function
   IO_BANK_BASE_STATUS = IO_BANK_BASE + 0x000,
   IO_BANK_BASE_CTRL = IO_BANK_BASE + 0x004 
};
// page: 42 of rp2040 datasheet
#define SIO_BASE 0xd0000000
enum {
    // reading
    GPIO_IN = SIO_BASE + 0x004,
    // writing
    GPIO_OUT = SIO_BASE + 0x010,
    GPIO_OUT_SET = SIO_BASE + 0x014,
    GPIO_OUT_CLR = SIO_BASE + 0x018,
    GPIO_OE = SIO_BASE + 0x20,
    GPIO_OE_CLR = SIO_BASE + 0x028,
    GPIO_OE_SET = SIO_BASE + 0x024
};

// page 203 of rp2040 datasheet
#define RESET_CONTROLLER 0x4000c000
enum {
    // 1 means reset 
    RESET = RESET_CONTROLLER,
    WATCH_DOG = RESET_CONTROLLER + 0x4,
    RESET_DONE = RESET_CONTROLLER + 0x8,
    RESETS_RESET_CLR = RESET_CONTROLLER + 0x3000,
};
// RESET register
enum {
    RESET_IO_BANK0 = 5,
    RESET_USBCTRL = 24, 
    RESET_UART1 = 23,
    RESET_UART0 = 22
};

// Checkout page 260 of rp2040 datasheet
typedef enum {
    SIO = 5,
    PIO0 = 6,
    PIO1 = 7,
} gpio_func_t;

// functions
enum {
    FUNC_SIO = 5,
    F2 = 2,
};


#define NUM_GPIOS 29
#define GPIO_IO_BANK_STATUS GPIO_IO_BANK_BASE + 0x000
#define GPIO_IO_BANK_CTRL GPIO_IO_BANK_BASE + 0x004


void PUT32(uint32_t pin, uint32_t val);
uint32_t GET32(uint32_t pin);
void DELAY(uint32_t val);

void gpio_reset();

void gpio_set_function(uint32_t pin, gpio_func_t func);

// Function F5 is SIO
void gpio_set_output(uint32_t pin);

void gpio_set_input(uint32_t pin);

void gpio_set_on(uint32_t pin);

void gpio_set_off(uint32_t pin);

void gpio_read(uint32_t pin);

// write either 1 or 0
void gpio_write(uint32_t pin, uint32_t val);

// set <pin> as a pullup
void gpio_set_pullup(unsigned pin);
// set <pin> as a pulldown.
void gpio_set_pulldown(unsigned pin);


#endif