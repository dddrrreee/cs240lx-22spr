#include <assert.h>

#include "rvsim.h"

void mem_write8(mem_t* m, uint32_t addr, int8_t val) {
    addr -= m->base;
    assert(addr < m->size);
    m->data[addr] = val;
}
void mem_write16(mem_t* m, uint32_t addr, int16_t val) {
    addr -= m->base;
    assert(addr < m->size);
    ((uint16_t*) m->data)[addr / sizeof(uint16_t)] = val;
}
void mem_write32(mem_t* m, uint32_t addr, int32_t val) {
    addr -= m->base;
    assert(addr < m->size);
    ((uint32_t*) m->data)[addr / sizeof(uint32_t)] = val;
}

int8_t mem_read8(mem_t* m, uint32_t addr) {
    addr -= m->base;
    assert(addr < m->size);
    return m->data[addr];
}
uint8_t mem_read8u(mem_t* m, uint32_t addr) {
    addr -= m->base;
    assert(addr < m->size);
    return m->data[addr];
}
int16_t mem_read16(mem_t* m, uint32_t addr) {
    addr -= m->base;
    assert(addr < m->size);
    return ((uint16_t*) m->data)[addr / sizeof(uint16_t)];
}
uint16_t mem_read16u(mem_t* m, uint32_t addr) {
    addr -= m->base;
    assert(addr < m->size);
    return ((uint16_t*) m->data)[addr / sizeof(uint16_t)];
}
int32_t mem_read32(mem_t* m, uint32_t addr) {
    addr -= m->base;
    assert(addr < m->size);
    return ((uint32_t*) m->data)[addr / sizeof(uint32_t)];
}
