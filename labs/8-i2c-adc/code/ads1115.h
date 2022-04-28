#ifndef __ADS1115_H__
#define __ADS1115_H__

void ads1115_reset(void);

void ads1115_write16(uint8_t dev_addr, uint8_t reg, uint16_t v);

uint16_t ads1115_read16(uint8_t dev_addr, uint8_t reg);

uint8_t ads1115_config(void);

// p27: register names
enum { conversion_reg = 0, config_reg = 1 };


#endif
