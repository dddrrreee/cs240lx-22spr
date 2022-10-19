#include "rpi.h"
#include "libc/bit-support.h"

#define I2S_REGS_BASE 0x20203000

// pins used by I2S peripheral
typedef enum {
    I2S_PIN_CLK = 18,
    I2S_PIN_FS = 19,
    I2S_PIN_DIN = 20,
    I2S_PIN_DOUT = 21,
} i2s_pin_t;

// I2S peripheral registers
// BCM peripherals page 125 (section 8.8)
typedef struct {
    uint32_t cs;        // control and status
    uint32_t fifo;      // fifo data 
    uint32_t mode;      // mode
    uint32_t rx_cfg;    // receive config
    uint32_t tx_cfg;    // transmit config
    uint32_t dreq;      // dma request level
    uint32_t int_en;    // interrupt enables
    uint32_t int_stc;   // interrupt status and clear
    uint32_t gray;      // gray mode control
} i2s_regs_t;

// make sure i2s_regs_t is setup correctly
_Static_assert(sizeof(i2s_regs_t)/sizeof(uint32_t) == 9, "i2s_regs_t size");
_Static_assert(offsetof(i2s_regs_t, cs) == 0x0, "i2s_regs_t cs offset");
_Static_assert(offsetof(i2s_regs_t, fifo) == 0x4, "i2s_regs_t fifo offset");
_Static_assert(offsetof(i2s_regs_t, mode) == 0x8, "i2s_regs_t mode offset");
_Static_assert(offsetof(i2s_regs_t, rx_cfg) == 0xc, "i2s_regs_t rx_cfg offset");
_Static_assert(offsetof(i2s_regs_t, tx_cfg) == 0x10, "i2s_regs_t tx_cfg offset");
_Static_assert(offsetof(i2s_regs_t, dreq) == 0x14, "i2s_regs_t dreq offset");
_Static_assert(offsetof(i2s_regs_t, int_en) == 0x18, "i2s_regs_t int_en offset");
_Static_assert(offsetof(i2s_regs_t, int_stc) == 0x1c, "i2s_regs_t int_stc offset");
_Static_assert(offsetof(i2s_regs_t, gray) == 0x20, "i2s_regs_t gray offset");

// timer divider (BCM peripherals page 108)
typedef enum {
    I2S_CLK_DIV_FRAC_LB = 0,
    I2S_CLK_DIV_FRAC_UB = 11,
    I2S_CLK_DIV_INT_LB = 12,
    I2S_CLK_DIV_INT_UB = 23
} i2s_div_bits_t;

// cs register bits (BCM peripherals page 126)
typedef enum {
    I2S_CS_EN = 0,
    I2S_CS_RXON = 1,
    I2S_CS_RXCLR = 4,
    I2S_CS_RXTHR_LB = 7,
    I2S_CS_RXTHR_UB = 8,
    I2S_CS_RXSYNC = 14,
    I2S_CS_RXERR = 16,
    I2S_CS_RXR = 18,
    I2S_CS_RXD = 20,
    I2S_CS_RXF = 22,
    I2S_CS_RXSEX = 23,
    I2S_CS_SYNC = 24,
    I2S_CS_STBY = 25,
} i2s_cs_bits_t;

// mode register bits (BCM peripherals page 131)
typedef enum {
    I2S_MODE_FSLEN_LB = 0,      // frame sync length
    I2S_MODE_FSLEN_UB = 9,
    I2S_MODE_FLEN_LB = 10,      // frame length
    I2S_MODE_FLEN_UB = 19
} i2s_mode_bits_t;

// rx control register bits (BCM peripherals page 132)
typedef enum {
    I2S_RXC_CH2WID_LB = 0,     // channel 2 width
    I2S_RXC_CH2WID_UB = 3,
    I2S_RXC_CH2POS_LB = 4,     // channel 2 position
    I2S_RXC_CH2POS_UB = 13,
    I2S_RXC_CH2EN = 14,         // channel 2 enable
    I2S_RXC_CH2WEX = 15,        // channel 2 width extension
    I2S_RXC_CH1WID_LB = 16,     // channel 1 width
    I2S_RXC_CH1WID_UB = 19,
    I2S_RXC_CH1POS_LB = 20,     // channel 1 position
    I2S_RXC_CH1POS_UB = 29,
    I2S_RXC_CH1EN = 30,         // channel 1 enable
    I2S_RXC_CH1WEX = 31,        // channel 1 width extension
} i2s_rxc_bits_t;



/////////////////////////
// CLOCK MANAGER STUFF //
/////////////////////////


// From errata (https://elinux.org/BCM2835_datasheet_errata#p107-108_table_6-35)
// See p107-108 table 6-35 section
// "Documentation relating to the PCM/I2S clock in missing"  ... yay :)
#define CM_REGS_BASE 0x20101000
#define CM_REGS_MSB 0x5A000000  // for some reason, we always need to write MS byte of CM regs with 0x5A
#define CM_CTRL_XTAL 0x01   // want to write 0x01 to CM_PCM_CTRL to use 19.2 MHz oscillator
#define CM_CTRL_EN (1 << 4) // bit 4 is enable bit
#define CM_CTRL_MASH3 (3 << 9) // bits 9-10 are MASH control, want 3 stage for best performance

// clock divider: 19.2 MHz / 6.8027 / 64 = 44.1001 KHz
#define CM_DIV_INT 6        // integer divider for 19.2 MHz clock
#define CM_DIV_FRAC 3288    // fractional divider for 19.2 MHz clock (experimentally determined)

typedef struct {
    uint32_t other_regs[0x26];  // don't care
    uint32_t pcm_ctrl;          // pcm clock control reg
    uint32_t pcm_div;           // pcm clock divider reg
} cm_regs_t;
_Static_assert(offsetof(cm_regs_t, pcm_ctrl) == 0x98, "cm_regs_t pcm_ctrl offset");
_Static_assert(offsetof(cm_regs_t, pcm_div) == 0x9C, "cm_regs_t pcm_div offset");




//////////////////////////
// ACTUAL I2S FUNCTIONS //
//////////////////////////


// initializes the I2S peripheral
void i2s_init(void);

// reads a sample from the I2S peripheral
int32_t i2s_read_sample(void);
