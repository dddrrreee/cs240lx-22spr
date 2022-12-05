#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rvsim.h"
#include "bits.h"

// Assigns 'reg' = 'val'. Does not every assign to register 0.
static void write_reg(machine_t* m, int reg, int32_t val) {
    if (reg == 0) {
        // cannot write register 0
        return;
    }
    m->regs[reg] = val;
}

// Branches to 'pc' if 'cond' is true.
static bool do_branch(machine_t* m, int32_t pc, bool cond) {
    if (cond) {
        m->pc = pc;
    }
    return cond;
}

// Returns the result of applying 'op' to 'a' and 'b'.
static int32_t alu_compute(int32_t a, int32_t b, alu_op_t op) {
    assert(!"unimplemented");
}

// Extracts the immediate from 'insn', assuming the immediate is encoded in the
// instruction with the corresponding type.
static uint32_t extract_imm(uint32_t insn, imm_type_t type) {
    // Implement the immediate decoder for each type of immediate encoding.
    assert(!"unimplemented");
}

#define RD(x) bits_get(x, 11, 7)
#define RS1(x) bits_get(x, 19, 15)
#define RS2(x) bits_get(x, 24, 20)
#define SHAMT(x) bits_get(x, 24, 20)
#define FUNCT3(x) bits_get(x, 14, 12)
#define FUNCT7(x) bits_get(x, 31, 25)

// Executes an R-type arithmetic instruction.
static void rarith(machine_t* m, uint32_t insn) {
    // Decode the ALU operation, rs1, rs2, and rd from the instruction. Then
    // run alu_compute and write the result back to the register file.
    assert(!"unimplemented");
}

// Executes an I-type arithmetic instruction.
static void iarith(machine_t* m, uint32_t insn) {
    // Decode the ALU operation, rs1, rd, and immediate. Then run alu_compute
    // and write the result back to the register file.
    // NOTE: for shift instructions, the immediate is encoded in the SHAMT
    // field (this matters for SRL).
    assert(!"unimplemented");
}

// Executes a branch instruction. Returns true if a jump occurred.
static bool branch(machine_t* m, uint32_t insn) {
    // Decode the immediate for a branch instruction.

    // Compute the condition for the jump.
    // HINT: you can use the ALU for this. Each branch instruction performs an
    // ALU computation, and then branches if the result is 0, or if the result
    // is not zero (depending on the branch type). First do the ALU operation,
    // (some branches use XOR, some use SLT, some use SLTU), and then compare
    // the result to 0 (some branches jump if equal to 0, and some jump if not
    // equal to 0).

    // If the condition is true, jump to pc + imm (and return whether you
    // jumped).

    assert(!"unimplemented");

    return false;
}

static void lui(machine_t* m, uint32_t insn) {
    // Load the U-type immediate into rd.
    assert(!"unimplemented");
}

static void auipc(machine_t* m, uint32_t insn) {
    // Decode the U-type immediate, add it to the PC, and write it into rd.
    assert(!"unimplemented");
}

// Example instruction: jal

static void jal(machine_t* m, uint32_t insn) {
    // Decode the immediate, compute the jump target (pc + imm), write pc + 4
    // to rd, and then do the jump.
    int32_t imm = extract_imm(insn, IMM_J);
    int32_t pc = m->pc + imm;
    write_reg(m, RD(insn), m->pc + 4);
    do_branch(m, pc, true);
}

static void jalr(machine_t* m, uint32_t insn) {
    // Similar to jal but the jump target is in rs1.
    assert(!"unimplemented");
}

static void load(machine_t* m, uint32_t insn) {
    // First compute the load address, which is rs1 + imm.

    // Next perform the read by use mem_readX(&m->mem, addr). The size of the
    // read is determined by the immext (funct3) type (to support all load
    // instructions: lb, lbu, lh, lhu, lw).

    // Write the value returned by the read to rd.
    assert(!"unimplemented");
}

static void store(machine_t* m, uint32_t insn) {
    // First compute the store address, which is rs1 + imm.

    // Perform the store with mem_writeX(&mm->mem, addr, rs2). Use the immext
    // (funct3) to determine the size of the store (sb, sh, sw).
    assert(!"unimplemented");
}

// Syscall handler: dispatches to the appriopriate syscall implementation in
// syscall.c. Returns true if the machine should halt.
static bool ecall(machine_t* m) {
    int sysno = m->regs[REG_A7];
    switch (sysno) {
        case SYSCALL_EXIT:
            return true;
        case SYSCALL_WRITE:
            m->regs[REG_A0] = sys_write(m, m->regs[REG_A0], m->regs[REG_A1], m->regs[REG_A2]);
            return false;
        case SYSCALL_BRK:
            m->regs[REG_A0] = sys_brk(m, m->regs[REG_A0]);
            return false;
        case SYSCALL_CLOSE:
            sys_close(m, m->regs[REG_A0]);
            return false;
        case SYSCALL_FSTAT:
            m->regs[REG_A0] = sys_fstat(m, m->regs[REG_A0], m->regs[REG_A1]);
            return false;
        default:
            printf("unknown syscall: %d\n", sysno);
            assert(false);
    }
    return false;
}

// Executes the next instruction. Returns true if the machine is done executing
// (halted).
bool machine_exec(machine_t* m) {
    uint32_t insn = mem_read32(&m->mem, m->pc);
    // for debug:
    // printf("pc: %x, exec: %x\n", m->pc, insn);

    bool jmp = false;
    bool halt = false;

    // Directly execute instructions that we don't have to decode.
    switch (insn) {
        case INSN_NOP:
            m->pc += 4;
            return false;
        case INSN_ECALL:
            halt = ecall(m);
            m->pc += 4;
            return halt;
        case INSN_EBREAK:
            return true;
    }

    // Decode the opcode, and dispatch to the appropriate helper function.
    assert(!"unimplemented");

    if (!jmp) {
        // if we didn't jump, increment pc to the next instruction.
        m->pc += 4;
    }

    return halt;
}

void machine_new(machine_t* m, uint32_t membase, uint32_t memsize) {
    m->mem = (mem_t){
        .data = (uint8_t*) malloc(memsize),
        .base = membase,
        .size = memsize,
    };
    assert(m->mem.data);
    memset(&m->regs, 0, sizeof(m->regs));
    // setup a stack at the top of memory.
    m->regs[REG_SP] = memsize - 16;
}

void machine_free(machine_t* m) {
    free(m->mem.data);
}
