#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    INSN_ECALL = 0x00000073,
    INSN_EBREAK = 0x00100073,
    INSN_NOP = 0x00000013,
} insn_t;

typedef enum {
    OP_RARITH = 0b0110011,
    OP_IARITH = 0b0010011,
    OP_BRANCH = 0b1100011,
    OP_LUI    = 0b0110111,
    OP_AUIPC  = 0b0010111,
    OP_JAL    = 0b1101111,
    OP_JALR   = 0b1100111,
    OP_LOAD   = 0b0000011,
    OP_STORE  = 0b0100011,
    OP_FENCE  = 0b0001111,
    OP_SYS    = 0b1110011,
} op_t;

typedef enum {
    // Alu ops are {funct7, funct3} for R-type instructions
    ALU_ADD = 0b0000000000,
    ALU_SUB = 0b0100000000,
    ALU_SLT = 0b0000000010,
    ALU_SLTU = 0b0000000011,
    ALU_XOR = 0b0000000100,
    ALU_SLL = 0b0000000001,
    ALU_SRL = 0b0000000101,
    ALU_SRA = 0b0100000101,
    ALU_OR = 0b00000000110,
    ALU_AND = 0b00000000111,
} alu_op_t;

typedef enum {
    IMM_I,
    IMM_S,
    IMM_B,
    IMM_J,
    IMM_U,
} imm_type_t;

typedef enum {
    EXT_BYTE  = 0b000,
    EXT_HALF  = 0b001,
    EXT_WORD  = 0b010,
    EXT_BYTEU = 0b100,
    EXT_HALFU = 0b101,
} imm_ext_t;

typedef enum {
    REG_ZERO = 0,
    REG_RA = 1,
    REG_SP = 2,
    REG_GP = 3,
    REG_TP = 4,
    REG_T0 = 5,
    REG_T1 = 6,
    REG_T2 = 7,
    REG_S0 = 8,
    REG_S1 = 9,
    REG_A0 = 10,
    REG_A1 = 11,
    REG_A2 = 12,
    REG_A3 = 13,
    REG_A4 = 14,
    REG_A5 = 15,
    REG_A6 = 16,
    REG_A7 = 17,
    REG_S2 = 18,
    REG_S3 = 19,
    REG_S4 = 20,
    REG_S5 = 21,
    REG_S6 = 22,
    REG_S7 = 23,
    REG_S8 = 24,
    REG_S9 = 25,
    REG_S10 = 26,
    REG_S11 = 27,
    REG_T3 = 28,
    REG_T4 = 29,
    REG_T5 = 30,
    REG_T6 = 31,
} reg_t;

typedef enum {
    SYSCALL_EXIT = 93,
    SYSCALL_WRITE = 64,
    SYSCALL_CLOSE = 57,
    SYSCALL_FSTAT = 80,
    SYSCALL_BRK = 214,
} syscall_t;

typedef struct {
    uint8_t* data;
    uint32_t base;
    uint32_t size;
} mem_t;

void mem_write8(mem_t* m, uint32_t addr, int8_t val);
void mem_write16(mem_t* m, uint32_t addr, int16_t val);
void mem_write32(mem_t* m, uint32_t addr, int32_t val);

int8_t mem_read8(mem_t* m, uint32_t addr);
uint8_t mem_read8u(mem_t* m, uint32_t addr);
int16_t mem_read16(mem_t* m, uint32_t addr);
uint16_t mem_read16u(mem_t* m, uint32_t addr);
int32_t mem_read32(mem_t* m, uint32_t addr);

typedef struct {
    int32_t pc;
    int32_t regs[32];
    mem_t mem;

    uint32_t brk;
} machine_t;

void machine_new(machine_t* m, uint32_t membase, uint32_t memsize);
void machine_free(machine_t* m);
bool machine_exec(machine_t* m);
void machine_load(machine_t* m, char* elfdat);

int sys_write(machine_t* m, int fd, uint32_t buf, uint32_t size);
int sys_close(machine_t* m, int fd);
uint32_t sys_brk(machine_t* m, uint32_t addr);
int sys_fstat(machine_t* m, int fd, uint32_t statbuf);
