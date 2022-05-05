#ifndef __ARMV6_ENCODINGS_H__
#define __ARMV6_ENCODINGS_H__
// engler, cs240lx: simplistic instruction encodings for r/pi ARMV6.
// this will compile both on our bare-metal r/pi and unix.

// bit better type checking to use enums.
enum {
    arm_r0 = 0, 
    arm_r1, 
    arm_r2,
    arm_r3,
    arm_r4,
    arm_r5,
    arm_r6,
    arm_r7,
    arm_r8,
    arm_r9,
    arm_r10,
    arm_r11,
    arm_r12,
    arm_r13,
    arm_r14,
    arm_r15,
    arm_sp = arm_r13,
    arm_lr = arm_r14,
    arm_pc = arm_r15
};
_Static_assert(arm_r15 == 15, "bad enum");


// condition code.
enum {
    arm_EQ = 0,
    arm_NE,
    arm_CS,
    arm_CC,
    arm_MI,
    arm_PL,
    arm_VS,
    arm_VC,
    arm_HI,
    arm_LS,
    arm_GE,
    arm_LT,
    arm_GT,
    arm_LE,
    arm_AL,
};
_Static_assert(arm_AL == 0b1110, "bad enum list");

// data processing op codes.
enum {
    arm_and_op = 0, 
    arm_eor_op,
    arm_sub_op,
    arm_rsb_op,
    arm_add_op,
    arm_adc_op,
    arm_sbc_op,
    arm_rsc_op,
    arm_tst_op,
    arm_teq_op,
    arm_cmp_op,
    arm_cmn_op,
    arm_orr_op,
    arm_mov_op,
    arm_bic_op,
    arm_mvn_op,
};
_Static_assert(arm_mvn_op == 0b1111, "bad num list");


enum {
	arm_bx_op = 102,
};
_Static_assert(arm_bx_op == 0b01100110, "bad num list");

/************************************************************
 * instruction encodings.  should do:
 *      bx lr
 *      ld *
 *      st *
 *      cmp
 *      branch
 *      alu 
 *      alu_imm
 *      jump
 *      call
 */


static inline void check_reg(uint8_t reg) {
	assert(reg >= 0);
	assert(reg < 16);
}
// add instruction:
//      add rdst, rs1, rs2
//  - general add instruction: page A4-6 [armv6.pdf]
//  - shift operatnd: page A5-8 [armv6.pdf]
//
// we do not do any carries, so S = 0.
static inline unsigned arm_add(uint8_t rd, uint8_t rs1, uint8_t rs2) {
    assert(arm_add_op == 0b0100);

	//make sure we arent passing bogus registers to the instruction
	check_reg(rd);
	check_reg(rs1);
	check_reg(rs2);

	/* add instruction encoding:
	 * |31 30 29 28| 27 26 | 25 | 24 23 22 21 | 20 | 19 18 17 16 | 15 14 13 12 | 11 10 9 8 7 6 5 4 3 2 1 0 |
	 * | arm_AL    | 0   0 |  0 | arm_add_op  | 0  |     rs1	 |      rd     |            rs2		       |
	 * | cond code |  SBZ  |  I | op cod      | S  |     Rn	     |      Rd     |        Shifter operand	   |
	 * see armv6 chapter A4  pg. A4-6
	 */
	uint32_t code = 0;
	code |= (arm_AL << 28); // always execute cond code
	code |= (arm_add_op << 21); //add op code
	code |= (rs1 << 16);
	code |= (rd << 12);
	code |= rs2;

	return code;
}

// <add> of an immediate
static inline uint32_t arm_add_imm8(uint8_t rd, uint8_t rs1, uint8_t imm) {
	assert(arm_add_op == 0b0100);

	//make sure we arent passing bogus registers to the instruction
	check_reg(rd);
	check_reg(rs1);

	/* add instruction encoding:
	 * |31 30 29 28| 27 26 | 25 | 24 23 22 21 | 20 | 19 18 17 16 | 15 14 13 12 | 11 10 9 8 7 6 5 4 3 2 1 0 |
	 * | arm_AL    | 0   0 |  1 | arm_add_op  | 0  |     rs1	 |      rd     |            imm		       |
	 * | cond code |  SBZ  |  I | op cod      | S  |     Rn	     |      Rd     |        Shifter operand	   |
	 * see armv6 chapter A4  pg. A4-6
	 */
	uint32_t code = 0;
	code |= (arm_AL << 28); // always execute cond code
	code |= (1 << 25); //signal use of immediate as shifter value
	code |= (arm_add_op << 21); //add op code
	code |= (rs1 << 16);
	code |= (rd << 12);
	code |= imm;

	return code;
}

static inline uint32_t arm_bx(uint8_t reg) {
	/* bx instruction encoding:
	 * |31 30 29 28| 27 26 25 24 23 22 21 20 | 19 18 17 16 | 15 14 13 12 | 11 10 9 8 | 7 6 5 4 3 2 1 0 |
	 * | arm_AL    | 		arm_bx_code      |      0      |      0      |      0     | 	arm_lr     |
	 * | cond code |          op code        |      SBO    |      SBO    |     SBO    |      Rm        |
	 * see armv6 chapter A4  pg. A4-6
	 */
	assert(reg == arm_lr);
	uint32_t code = 0;
	code |= (arm_AL << 28);
	code |= (arm_bx_op << 20);
	code |= (0xFFF << 8); //SBO: Should-be-one
	code |= (reg);
	return code;
}


// // <ldr> load a value from memory into a  register using immediate offset addr mode
// static inline uint32_t arm_ldr_imm_off(uint8_t rd, uint8_t rn, uint16_t addr_mode) {
// 	assert(arm_add_op == 0b0100);

// 	//make sure we arent passing bogus registers to the instruction
// 	check_reg(rd);
// 	check_reg(rn);

// 	/* add instruction encoding:
// 	 * |31 30 29 28| 27 26 | 25 | 24 | 23 | 22 |  21 | 20 | 19 18 17 16 | 15 14 13 12 | 11 10 9 8 7 6 5 4 3 2 1 0 |
// 	 * | arm_AL    | 0   1 |  1 | 1  |0/1 |  0 | 	 |      rd     |            imm		       |
// 	 * | cond code |SBZ/SBO|  I | P  | U  | SBZ|  W  |
// 	 * see armv6 chapter A4  pg. A4-6
// 	 */
// 	uint32_t code = 0;
// 	code |= (arm_AL << 28); // always execute cond code
// 	code |= (0b01 << 26); 	// 
// 	code |= (arm_add_op << 21); //add op code
// 	code |= (rs1 << 16);
// 	code |= (rd << 12);
// 	code |= imm;

// 	return code;
// }

// load an or immediate and rotate it.
static inline uint32_t 
arm_or_imm_rot(uint8_t rd, uint8_t rs1, uint8_t imm8, uint8_t rot_nbits) {
    unimplemented();
}

#endif
