#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rv_emu.h"
#include "bit.h"

#define DEBUG 0

static void unsupported(char *s, uint32_t n) {
    printf("unsupported %s 0x%x\n", s, n);
    exit(-1);
}

void rv_init(struct rv_state_st *rsp, uint32_t *func,
             uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3) {
    int i;

    // Zero out registers
    for (i = 0; i < RV_NREGS; i += 1) {
        rsp->regs[i] = 0;
    }

    // Zero out the stack
    for (i = 0; i < STACK_SIZE; i += 1) {
        rsp->stack[i] = 0;
    }

    // Initialize the Program Counter
    rsp->pc = (uint64_t) func;

    // Register 0 (zero) is always 0
    rsp->regs[RV_ZERO] = 0;

    // Initialize the Link Register to a sentinel value
    rsp->regs[RV_RA] = 0;

    // Initialize Stack Pointer to the logical bottom of the stack
    rsp->regs[RV_SP] = (uint64_t) &rsp->stack[STACK_SIZE];

    // Initialize the first 4 arguments in emulated r0-r3
    rsp->regs[RV_A0] = a0;
    rsp->regs[RV_A1] = a1;
    rsp->regs[RV_A2] = a2;
    rsp->regs[RV_A3] = a3;


    memset(&rsp->analysis, 0, sizeof(struct analysis_st));
    cache_init(&rsp->i_cache);
}

void emu_r_type(struct rv_state_st *rsp, uint32_t iw) {
    uint32_t rd = get_bits(iw, 7, 5); // (iw >> 7) & 0b11111;
    uint32_t rs1 = get_bits(iw, 15, 5); // (iw >> 15) & 0b11111;
    uint32_t rs2 = get_bits(iw, 20, 5); // (iw >> 20) & 0b11111;
    uint32_t funct3 = get_bits(iw, 12, 3); // (iw >> 12) & 0b111;
    uint32_t funct7 = get_bits(iw, 25, 7); // (iw >> 25) & 0b1111111;

   if (funct3 == 0b000 && funct7 == 0b0000000) { // add
        rsp->regs[rd] = rsp->regs[rs1] + rsp->regs[rs2];
    } else if(funct3 == 0b111 && funct7 == 0b0000000){ // and
    	rsp->regs[rd] = rsp->regs[rs1] & rsp->regs[rs2];
    } else if(funct3 == 0b000 && funct7 == 0b0100000){ // sub
    	rsp->regs[rd] = rsp->regs[rs1] - rsp->regs[rs2];
    } else if(funct3 == 0b001 && funct7 == 0b0000000){ // sll
    	rsp->regs[rd] = rsp->regs[rs1] << rsp->regs[rs2];
    } else if(funct3 == 0b101 && funct7 == 0b0000000){ // srl
    	rsp->regs[rd] = rsp->regs[rs1] >> rsp->regs[rs2];
    } else if(funct3 == 0b000 && funct7 == 0b0000001){ // mul
    	rsp->regs[rd] = rsp->regs[rs1] * rsp->regs[rs2];
    } else {
        unsupported("R-type funct3", funct3);
    }
    rsp->pc += 4; // Next instruction
}

void emu_r_type_64(struct rv_state_st *rsp, uint32_t iw){
	uint32_t rd = get_bits(iw, 7, 5);
    uint32_t rs1 = get_bits(iw, 15, 5);
    uint32_t rs2 = get_bits(iw, 20, 5);
    uint32_t funct3 = get_bits(iw, 12, 3);
    uint32_t funct7 = get_bits(iw, 25, 7);
	uint8_t shamt = (uint8_t) get_bits(rsp->regs[rs2], 0, 5);

   if (funct3 == 0b001 && funct7 == 0b0000000) { // sllw
        rsp->regs[rd] = rsp->regs[rs1] << shamt;
    } else if(funct3 == 0b101 && funct7 == 0b0100000){ // sraw
    	rsp->regs[rd] = (int64_t) sign_extend(rsp->regs[rs1], 31) >> shamt;
    } else {
        unsupported("R-type-64 funct3", funct3);
    }
    rsp->pc += 4; // Next instruction
}

void emu_i_type_load(struct rv_state_st *rsp, uint32_t iw) {
    uint32_t rd = get_bits(iw, 7, 5);
    uint32_t rs1 = get_bits(iw, 15, 5);
    uint32_t funct3 = get_bits(iw, 12, 3);

    int32_t imm32 = ((int32_t) iw) >> 20;
    int64_t imm64 = imm32;

    if (funct3 == 0b010) { // lw
    	int *address = (int*) (imm64 + rsp->regs[rs1]);
    	rsp->regs[rd] = *address;
    } else if(funct3 == 0b000){ // lb
    	char *address = (char*) (imm64 + rsp->regs[rs1]);
    	rsp->regs[rd] = *address;
    } else if(funct3 == 0b011){ // ld
    	double *address = (double*) (imm64 + rsp->regs[rs1]);
    	rsp->regs[rd] = *address;
    } else {
    	unsupported("I-type-load funct3", funct3);
    }
    rsp->pc += 4; // Next instruction
}

void emu_jalr(struct rv_state_st *rsp, uint32_t iw) {
    uint32_t rs1 = get_bits(iw, 15, 5); // Will be ra (aka x1)
    uint32_t imm11_0 = get_bits(iw, 20, 12);
    int64_t offset = sign_extend(imm11_0, 11);

    uint64_t val = rsp->regs[rs1] + offset;  // Value of regs[1]
    val &= ~(uint64_t) 1;
    rsp->regs[rs1] = rsp->pc + 4;
    rsp->pc = val;  // PC = return address
}

void emu_i_type(struct rv_state_st *rsp, uint32_t iw) {
    uint32_t rd = get_bits(iw, 7, 5);
    uint32_t rs1 = get_bits(iw, 15, 5);
    uint32_t funct3 = get_bits(iw, 12, 3);

    int32_t imm32 = ((int32_t) iw) >> 20;
    int64_t imm64 = imm32;

    if (funct3 == 0b000) { // addi
        rsp->regs[rd] = rsp->regs[rs1] + imm64;
    } else if (funct3 == 0b101) { // srli
    	uint8_t shamt = (uint8_t) imm64 & 0b11111;
        rsp->regs[rd] = rsp->regs[rs1] >> shamt;
    } else {
    	unsupported("I-type funct3", funct3);
    }
    rsp->pc += 4; // Next instruction
}

void emu_s_type(struct rv_state_st *rsp, uint32_t iw){
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);
	uint32_t imm4_0 = get_bits(iw, 7, 5);
	uint32_t imm11_5 = get_bits(iw, 25, 7) << 5;
	uint64_t temp = imm11_5 | imm4_0;
	int64_t offset = sign_extend(temp, 11);

	if (funct3 == 0b010) { // sw
    	int *address = (int*) (offset + rsp->regs[rs1]);
    	*address = rsp->regs[rs2];
    } else if(funct3 == 0b000){ //sb
    	char *address = (char*) (offset + rsp->regs[rs1]);
    	*address = rsp->regs[rs2];
    } else if(funct3 == 0b011){ // sd
    	double *address = (double*) (offset + rsp->regs[rs1]);
    	*address = rsp->regs[rs2];
    } else {
    	unsupported("S-type funct3", funct3);
    }
	rsp->pc += 4;
}

void emu_b_type(struct rv_state_st *rsp, uint32_t iw) {
    uint32_t rs1 = get_bits(iw, 15, 5);
    uint32_t rs2 = get_bits(iw, 20, 5);
    uint32_t funct3 = get_bits(iw, 12, 3);
    uint32_t imm12 = get_bits(iw, 31, 1) << 12;
    uint32_t imm10_5 = get_bits(iw, 25, 6) << 5;
    uint32_t imm4_1 = get_bits(iw, 8, 4) << 1;
    uint32_t imm11 = get_bits(iw, 7, 1) << 11;
    uint64_t temp = imm12 | imm10_5 | imm4_1 | imm11;
    int64_t offset = sign_extend(temp, 12);

    int64_t reg_1 = (int64_t) rsp->regs[rs1];
    int64_t reg_2 = (int64_t) rsp->regs[rs2];

    if (funct3 == 0b000) { // beq
    	if(reg_1 == reg_2){
    		rsp->pc += (int) offset;
    		rsp->analysis.b_taken += 1;
    	} else{
    		rsp->pc += 4;
    		rsp->analysis.b_not_taken += 1;
    	}
    } else if(funct3 == 0b001){ // bne
    	if(reg_1 != reg_2){
    		rsp->pc += (int) offset;
			rsp->analysis.b_taken += 1;
    	}else{
    		rsp->pc += 4;
    		rsp->analysis.b_not_taken += 1;
    	}
    } else if(funct3 == 0b100){ // blt
    	if(reg_1 < reg_2){
    		rsp->pc += (int) offset;
			rsp->analysis.b_taken += 1;
    	} else{
    		rsp->pc += 4;
    		rsp->analysis.b_not_taken += 1;
    	}
    } else if(funct3 == 0b101){ // bge
    	if(reg_1 >= reg_2){
    		rsp->pc += (int) offset;
    		rsp->analysis.b_taken += 1;
    	} else{
    		rsp->pc += 4;
    		rsp->analysis.b_not_taken += 1;
    	}
    } else {
        unsupported("B-type funct3", funct3);
    }
}

void emu_jal(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
    uint32_t imm20 = get_bits(iw, 31, 1) << 20;
    uint32_t imm10_1 = get_bits(iw, 21, 10) << 1;
    uint32_t imm11 = get_bits(iw, 20, 1) << 11;
    uint32_t imm19_12 = get_bits(iw, 12, 8) << 12;
    uint64_t temp = imm20 | imm19_12 | imm11 | imm10_1;
    int64_t offset = sign_extend(temp, 20);

    if(rd != 0){
    	rsp->regs[rd] = rsp->pc + 4;
    }
    rsp->pc += offset;
}

static void rv_one(struct rv_state_st *rsp) {
    uint32_t iw  = *((uint32_t*) rsp->pc);
    iw = cache_lookup(&rsp->i_cache, (uint64_t) rsp->pc);

    uint32_t opcode = get_bits(iw, 0, 7);

#if DEBUG
    printf("iw: %08x\n", iw);
#endif

	/*
	FMT_R       = 0b0110011,
	FMT_R_64    = 0b0111011
    FMT_I_LOAD  = 0b0000011,
    FMT_I_JALR  = 0b1100111,
    FMT_I_ARITH = 0b0010011,
    FMT_S       = 0b0100011,
    FMT_B       = 0b1100011,
    FMT_J       = 0b1101111
    */

    switch (opcode) {
        case FMT_R:
            // R-type instructions have two register operands
            emu_r_type(rsp, iw);
            rsp->analysis.ir_count += 1;
            break;
        case FMT_R_64:
        	emu_r_type_64(rsp, iw);
        	rsp->analysis.ir_count += 1;
        	break;
		case FMT_I_LOAD:
			// I-type load
			emu_i_type_load(rsp, iw);
			rsp->analysis.ld_count += 1;
			break;
        case FMT_I_JALR:
            // JALR (aka RET) is a variant of I-type instructions
            emu_jalr(rsp, iw);
            rsp->analysis.j_count += 1;
            break;
        case FMT_I_ARITH:
        	// I-type
        	emu_i_type(rsp, iw);
        	rsp->analysis.ir_count += 1;
        	break;
		case FMT_S:
			// S-type
			emu_s_type(rsp, iw);
			rsp->analysis.st_count += 1;
			break;
		case FMT_B:
			// B-type
			emu_b_type(rsp, iw);
			break;
		case FMT_J:
			emu_jal(rsp, iw);
			rsp->analysis.j_count += 1;
			break;
        default:
            unsupported("Unknown opcode: ", opcode);
    }
}

uint64_t rv_emulate(struct rv_state_st *rsp) {
    while (rsp->pc != RV_STOP) {
    	rsp->analysis.i_count += 1;
        rv_one(rsp);
    }
    return rsp->regs[RV_A0];
}

static void print_pct(char *fmt, int numer, int denom) {
    double pct = 0.0;

    if (denom)
        pct = (double) numer / (double) denom * 100.0;
    printf(fmt, numer, pct);
}

void rv_print(struct analysis_st *ap) {
    int b_total = ap->b_taken + ap->b_not_taken;

    printf("=== Analysis\n");
    print_pct("Instructions Executed  = %d\n", ap->i_count, ap->i_count);
    print_pct("R-type + I-type        = %d (%.2f%%)\n", ap->ir_count, ap->i_count);
    print_pct("Loads                  = %d (%.2f%%)\n", ap->ld_count, ap->i_count);
    print_pct("Stores                 = %d (%.2f%%)\n", ap->st_count, ap->i_count);    
    print_pct("Jumps/JAL/JALR         = %d (%.2f%%)\n", ap->j_count, ap->i_count);
    print_pct("Conditional branches   = %d (%.2f%%)\n", b_total, ap->i_count);
    print_pct("  Branches taken       = %d (%.2f%%)\n", ap->b_taken, b_total);
    print_pct("  Branches not taken   = %d (%.2f%%)\n", ap->b_not_taken, b_total);
}
