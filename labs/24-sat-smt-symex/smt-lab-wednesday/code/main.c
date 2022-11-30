#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include "smt.h"

/**** GLOBALS ****/

// This is the bitvector width to use, i.e., the word size of the machine. Note
// that the underlying SMT library will start to fail if this is greater than
// 32, and will *definitely* fail if this is greater than 63.
static const int WORD_SIZE = 4;

// Each instruction in our program has one of these opcodes
enum op {
    OP_EXIT = 0,    // End program
    OP_IMMEDIATE,   // Copy an immediate into a register
    OP_MOVE,        // Copy one register into another
    OP_STORE,       // Move one register into the memory pointed to by another
    OP_LOAD,        // Move memory pointed to by one register into another
    OP_BRANCH_EQ,   // If the two registers have identical values, jump
    OP_FAIL,        // If this point is reached, there is a bug
    OP_ADD,         // Addition of registers
};

static const char *op_names[] = {
    "exit", "immediate", "move", "store", "load", "branch_eq", "fail", "add",
};

// An instruction is an opcode and some arguments
struct instruction {
    enum op op;
    int64_t args[3];
};

// We store the program as an array of instructions
static struct instruction *PROGRAM = NULL;
static int N_PROGRAM = 0;

// An input variable is a register whose value we use without ever loading or
// moving a value into it, *OR* a memory read that hasn't been read before. We
// use this list to print out just the parts of the solution that we need (the
// idea is everything else is just a function of these). Every initial read
// from an unassigned register is stored here, along with every memory
// load/store (they will be dedup'd at the printing stage).
struct in_var {
    int register_number;
    int bv_id;

    // If this is a memory lookup...
    int bv_key, is_store;
};
static struct in_var *IN_VARS = NULL;
static int N_IN_VARS = 0;

static int *REGS = NULL, N_REGS = 0;
static void set_register(int n, int val) {
    while (n >= N_REGS)
        APPEND_GLOBAL(REGS) = -1;
    REGS[n] = val;
}

static int get_register(int n) {
    if (n < N_REGS && REGS[n] >= 0) return REGS[n];
    set_register(n, new_bv(WORD_SIZE));
    APPEND_GLOBAL(IN_VARS) = (struct in_var){
        .register_number = n,
        .bv_id = REGS[n],
    };
    return REGS[n];
}

/**** main symbolic interpreter ****/
int main() {
    // Parse the input program
    char opcode[20];
    int64_t args[3];
    while (!feof(stdin)) {
        memset(args, 0, sizeof(args));
        assert(scanf("%s %ld %ld %ld ", opcode, &(args[0]), &(args[1]), &(args[2])));
        if (!strcmp(opcode, ";")) {
            while (getc(stdin) != '\n');
            continue;
        }
        enum op op = OP_EXIT;
        for (int i = 0; i < sizeof(op_names) / sizeof(op_names[0]); i++) {
            if (!strcmp(opcode, op_names[i])) {
                op = i;
                break;
            }
        }
        APPEND_GLOBAL(PROGRAM).op = op;
        memcpy(PROGRAM[N_PROGRAM - 1].args, args, sizeof(args));
    }

    // Create an array to represent program memory
    int array_memory = new_array();

    // Do the symbolic interpretation. We generally use dst src ordering.
    int pc = 0;
    while (pc < N_PROGRAM) {
        struct instruction instruction = PROGRAM[pc++];
        printf("[%d] %s\n", getpid(), op_names[instruction.op]);
        int bv_lhs, bv_rhs, bv_dst_addr, bv_src;
        switch (instruction.op) {
            // Register Operations
            case OP_IMMEDIATE:  // immediate dstreg value
                assert(!"Implement me");
                break;
            case OP_MOVE:       // move dstreg srcreg
                assert(!"Implement me");
                break;
            case OP_ADD:        // add dstreg op1reg op2reg
                assert(!"Implement me");
                break;

            // Memory operations
            case OP_STORE:      // store addrreg valuereg
                // Get a bv for the destination address register and source
                // register value, then do a set to memory.
                // IMPORTANT: Afterwards, append to IN_VARS like so:
                // APPEND_GLOBAL(IN_VARS) = (struct in_var){
                //     .register_number = -1,
                //     .bv_key = bv_dst_addr,
                //     .is_store = 1,
                // };
                assert(!"Implement me");
                break;
            case OP_LOAD:       // load addrreg dstvaluereg
                // Get a bv for the source address register, do a get from
                // memory, then set the destination register to the result.
                // IMPORTANT: Afterwards, append to IN_VARS like so:
                // APPEND_GLOBAL(IN_VARS) = (struct in_var){
                //     .register_number = -1,
                //     .bv_id = bv_dst_register_value,
                //     .bv_key = bv_source_address_register,
                //     .is_store = 0,
                // };
                assert(!"Implement me");
                break;

            // Branching
            case OP_BRANCH_EQ:  // branch_eq op1 op2 offset_if_eq
                // Get bvs for the two register arguments. Fork, asserting
                // equality/disequality in each process and update the pc as
                // necessary. Note that, if the values are equal, we should
                // jump to the pc of this instruction + offset_if_eq. But above
                // we've already done pc++, so you need one fewer.
                assert(!"Implement me");
                break;

            // Success & failure
            case OP_FAIL:
                if (solve()) {
                    printf("[%d] Found solution:\n", getpid());
                    for (int i = 0; i < N_IN_VARS; i++) {
                        if (IN_VARS[i].register_number >= 0) {
                            printf("\t[%d] Register %d = %ld\n", getpid(),
                                   IN_VARS[i].register_number,
                                   get_solution(IN_VARS[i].bv_id, 0));
                        } else if (IN_VARS[i].is_store) {
                            continue;
                        } else {
                            int key = get_solution(IN_VARS[i].bv_key, 0);
                            // Make sure we haven't read from/wrote to this key
                            // before
                            int found = 0;
                            for (int j = 0; j < i; j++) {
                                if (IN_VARS[j].register_number == -1
                                        && get_solution(IN_VARS[j].bv_key, 0) == key) {
                                    found = 1;
                                    break;
                                }
                            }
                            if (found) continue;
                            printf("\t[%d] Memory[%d] = %ld\n", getpid(),
                                   key, get_solution(IN_VARS[i].bv_id, 0));
                        }
                    }
                }
            case OP_EXIT:
                return 0;
        }
    }
    return 0;
}
