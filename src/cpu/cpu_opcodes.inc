/**
 * @file opcodes.inc
 * @brief Contains the definitions for all the CPU opcodes. This is a very repetitive and long file.
 */
#pragma once

#include "DEFINES.h"

static void cpu_opcode_undefined(void);
static void cpu_opcode_illegal(void);

// ========== Standard Opcodes ==========

// ----- Control -----

// Misc
static void cpu_opcode_NOP(void);
static void cpu_opcode_STOP(void);
static void cpu_opcode_HALT(void);
static void cpu_opcode_CB(void);
static void cpu_opcode_DI(void);
static void cpu_opcode_EI(void);
// Misc

// Branch
static void cpu_opcode_JR_s8(void);
static void cpu_opcode_JR_NZ_s8(void);
static void cpu_opcode_JR_NC_s8(void);
static void cpu_opcode_JR_Z_s8(void);
static void cpu_opcode_JR_C_s8(void);
static void cpu_opcode_RET(void);
static void cpu_opcode_RET_NZ(void);
static void cpu_opcode_RET_NC(void);
static void cpu_opcode_RET_Z(void);
static void cpu_opcode_RET_C(void);
static void cpu_opcode_RETI(void);
static void cpu_opcode_JP_u16(void);
static void cpu_opcode_JP_NZ_u16(void);
static void cpu_opcode_JP_NC_u16(void);
static void cpu_opcode_JP_Z_u16(void);
static void cpu_opcode_JP_C_u16(void);
static void cpu_opcode_JP_rHL(void);
static void cpu_opcode_CALL_u16(void);
static void cpu_opcode_CALL_NZ_u16(void);
static void cpu_opcode_CALL_NC_u16(void);
static void cpu_opcode_CALL_Z_u16(void);
static void cpu_opcode_CALL_C_u16(void);
static void cpu_opcode_RST_00h(void);
static void cpu_opcode_RST_08h(void);
static void cpu_opcode_RST_10h(void);
static void cpu_opcode_RST_18h(void);
static void cpu_opcode_RST_20h(void);
static void cpu_opcode_RST_28h(void);
static void cpu_opcode_RST_30h(void);
static void cpu_opcode_RST_38h(void);
// Branch

// ----- Control -----

// ----- 8-bit Load/Store/Move -----

// Load immediate into register
static void cpu_opcode_LD_rB_u8(void);
static void cpu_opcode_LD_rC_u8(void);
static void cpu_opcode_LD_rD_u8(void);
static void cpu_opcode_LD_rE_u8(void);
static void cpu_opcode_LD_rH_u8(void);
static void cpu_opcode_LD_rL_u8(void);
static void cpu_opcode_LD_iHL_u8(void);
static void cpu_opcode_LD_rA_u8(void);

// Load indirectly
static void cpu_opcode_LD_iBC_rA(void);
static void cpu_opcode_LD_iDE_rA(void);
static void cpu_opcode_LD_iHLi_rA(void);
static void cpu_opcode_LD_iHLd_rA(void);
static void cpu_opcode_LD_rA_iBC(void);
static void cpu_opcode_LD_rA_iDE(void);
static void cpu_opcode_LD_rA_iHLi(void);
static void cpu_opcode_LD_rA_iHLd(void);

// Load B from Register (or indirect HL)
static void cpu_opcode_LD_rB_rB(void);
static void cpu_opcode_LD_rB_rC(void);
static void cpu_opcode_LD_rB_rD(void);
static void cpu_opcode_LD_rB_rE(void);
static void cpu_opcode_LD_rB_rH(void);
static void cpu_opcode_LD_rB_rL(void);
static void cpu_opcode_LD_rB_iHL(void);
static void cpu_opcode_LD_rB_rA(void);

// Load C from Register (or indirect HL)
static void cpu_opcode_LD_rC_rB(void);
static void cpu_opcode_LD_rC_rC(void);
static void cpu_opcode_LD_rC_rD(void);
static void cpu_opcode_LD_rC_rE(void);
static void cpu_opcode_LD_rC_rH(void);
static void cpu_opcode_LD_rC_rL(void);
static void cpu_opcode_LD_rC_iHL(void);
static void cpu_opcode_LD_rC_rA(void);

// Load D from Register (or indirect HL)
static void cpu_opcode_LD_rD_rB(void);
static void cpu_opcode_LD_rD_rC(void);
static void cpu_opcode_LD_rD_rD(void);
static void cpu_opcode_LD_rD_rE(void);
static void cpu_opcode_LD_rD_rH(void);
static void cpu_opcode_LD_rD_rL(void);
static void cpu_opcode_LD_rD_iHL(void);
static void cpu_opcode_LD_rD_rA(void);

// Load E from Register (or indirect HL)
static void cpu_opcode_LD_rE_rB(void);
static void cpu_opcode_LD_rE_rC(void);
static void cpu_opcode_LD_rE_rD(void);
static void cpu_opcode_LD_rE_rE(void);
static void cpu_opcode_LD_rE_rH(void);
static void cpu_opcode_LD_rE_rL(void);
static void cpu_opcode_LD_rE_iHL(void);
static void cpu_opcode_LD_rE_rA(void);

// Load H from Register (or indirect HL)
static void cpu_opcode_LD_rH_rB(void);
static void cpu_opcode_LD_rH_rC(void);
static void cpu_opcode_LD_rH_rD(void);
static void cpu_opcode_LD_rH_rE(void);
static void cpu_opcode_LD_rH_rH(void);
static void cpu_opcode_LD_rH_rL(void);
static void cpu_opcode_LD_rH_iHL(void);
static void cpu_opcode_LD_rH_rA(void);

// Load L from Register (or indirect HL)
static void cpu_opcode_LD_rL_rB(void);
static void cpu_opcode_LD_rL_rC(void);
static void cpu_opcode_LD_rL_rD(void);
static void cpu_opcode_LD_rL_rE(void);
static void cpu_opcode_LD_rL_rH(void);
static void cpu_opcode_LD_rL_rL(void);
static void cpu_opcode_LD_rL_iHL(void);
static void cpu_opcode_LD_rL_rA(void);

// Load indirect HL from Register
static void cpu_opcode_LD_iHL_rB(void);
static void cpu_opcode_LD_iHL_rC(void);
static void cpu_opcode_LD_iHL_rD(void);
static void cpu_opcode_LD_iHL_rE(void);
static void cpu_opcode_LD_iHL_rH(void);
static void cpu_opcode_LD_iHL_rL(void);
// No LD (HL), (HL) instruction
static void cpu_opcode_LD_iHL_rA(void);

// Load A from Register (or indirect HL)
static void cpu_opcode_LD_rA_rB(void);
static void cpu_opcode_LD_rA_rC(void);
static void cpu_opcode_LD_rA_rD(void);
static void cpu_opcode_LD_rA_rE(void);
static void cpu_opcode_LD_rA_rH(void);
static void cpu_opcode_LD_rA_rL(void);
static void cpu_opcode_LD_rA_iHL(void);
static void cpu_opcode_LD_rA_rA(void);

// Load High
// Used to quickly access IO registers
// Note: These can be tricky to read but are always in reference to A
static void cpu_opcode_LDH_u8_rA(void);
static void cpu_opcode_LDH_rA_u8(void);
static void cpu_opcode_LDH_rC_rA(void);
static void cpu_opcode_LDH_rA_rC(void);

// Load indirect immediate
static void cpu_opcode_LD_i16_rA(void);
static void cpu_opcode_LD_rA_i16(void);

// ----- 8-bit Load/Store/Move -----

// ----- 16-bit Load/Store/Mode ----

// Load immediate
static void cpu_opcode_LD_rBC_u16(void);
static void cpu_opcode_LD_rDE_u16(void);
static void cpu_opcode_LD_rHL_u16(void);
static void cpu_opcode_LD_rSP_u16(void);

// Special SP
static void cpu_opcode_LD_i16_rSP(void);
static void cpu_opcode_LD_rSP_rHL(void);

// POP/PUSH
static void cpu_opcode_POP_rBC(void);
static void cpu_opcode_POP_rDE(void);
static void cpu_opcode_POP_rHL(void);
static void cpu_opcode_POP_rAF(void);
static void cpu_opcode_PUSH_rBC(void);
static void cpu_opcode_PUSH_rDE(void);
static void cpu_opcode_PUSH_rHL(void);
static void cpu_opcode_PUSH_rAF(void);

// ----- 16-bit Load/Store/Move -----

// ----- 8-bit ALU -----

// Increment Register (or indirect HL)
static void cpu_opcode_INC_rB(void);
static void cpu_opcode_INC_rC(void);
static void cpu_opcode_INC_rD(void);
static void cpu_opcode_INC_rE(void);
static void cpu_opcode_INC_rH(void);
static void cpu_opcode_INC_rL(void);
static void cpu_opcode_INC_iHL(void);
static void cpu_opcode_INC_rA(void);

// Decrement Register (or indirect HL)
static void cpu_opcode_DEC_rB(void);
static void cpu_opcode_DEC_rC(void);
static void cpu_opcode_DEC_rD(void);
static void cpu_opcode_DEC_rE(void);
static void cpu_opcode_DEC_rH(void);
static void cpu_opcode_DEC_rL(void);
static void cpu_opcode_DEC_iHL(void);
static void cpu_opcode_DEC_rA(void);

// Add Register (or indirect HL) to A
static void cpu_opcode_ADD_rA_rB(void);
static void cpu_opcode_ADD_rA_rC(void);
static void cpu_opcode_ADD_rA_rD(void);
static void cpu_opcode_ADD_rA_rE(void);
static void cpu_opcode_ADD_rA_rH(void);
static void cpu_opcode_ADD_rA_rL(void);
static void cpu_opcode_ADD_rA_iHL(void);
static void cpu_opcode_ADD_rA_rA(void);

// Add with carry Register (or indirect HL) to A
static void cpu_opcode_ADC_rA_rB(void);
static void cpu_opcode_ADC_rA_rC(void);
static void cpu_opcode_ADC_rA_rD(void);
static void cpu_opcode_ADC_rA_rE(void);
static void cpu_opcode_ADC_rA_rH(void);
static void cpu_opcode_ADC_rA_rL(void);
static void cpu_opcode_ADC_rA_iHL(void);
static void cpu_opcode_ADC_rA_rA(void);

// Subtract Register (or indirect HL) from A
static void cpu_opcode_SUB_rA_rB(void);
static void cpu_opcode_SUB_rA_rC(void);
static void cpu_opcode_SUB_rA_rD(void);
static void cpu_opcode_SUB_rA_rE(void);
static void cpu_opcode_SUB_rA_rH(void);
static void cpu_opcode_SUB_rA_rL(void);
static void cpu_opcode_SUB_rA_iHL(void);
static void cpu_opcode_SUB_rA_rA(void);

// Subtract with carry Register (or indirect HL) from A
static void cpu_opcode_SBC_rA_rB(void);
static void cpu_opcode_SBC_rA_rC(void);
static void cpu_opcode_SBC_rA_rD(void);
static void cpu_opcode_SBC_rA_rE(void);
static void cpu_opcode_SBC_rA_rH(void);
static void cpu_opcode_SBC_rA_rL(void);
static void cpu_opcode_SBC_rA_iHL(void);
static void cpu_opcode_SBC_rA_rA(void);

// AND Register (or indirect HL) with A
static void cpu_opcode_AND_rA_rB(void);
static void cpu_opcode_AND_rA_rC(void);
static void cpu_opcode_AND_rA_rD(void);
static void cpu_opcode_AND_rA_rE(void);
static void cpu_opcode_AND_rA_rH(void);
static void cpu_opcode_AND_rA_rL(void);
static void cpu_opcode_AND_rA_iHL(void);
static void cpu_opcode_AND_rA_rA(void);

// XOR Register (or indirect HL) with A
static void cpu_opcode_XOR_rA_rB(void);
static void cpu_opcode_XOR_rA_rC(void);
static void cpu_opcode_XOR_rA_rD(void);
static void cpu_opcode_XOR_rA_rE(void);
static void cpu_opcode_XOR_rA_rH(void);
static void cpu_opcode_XOR_rA_rL(void);
static void cpu_opcode_XOR_rA_iHL(void);
static void cpu_opcode_XOR_rA_rA(void);

// OR Register (or indirect HL) with A
static void cpu_opcode_OR_rA_rB(void);
static void cpu_opcode_OR_rA_rC(void);
static void cpu_opcode_OR_rA_rD(void);
static void cpu_opcode_OR_rA_rE(void);
static void cpu_opcode_OR_rA_rH(void);
static void cpu_opcode_OR_rA_rL(void);
static void cpu_opcode_OR_rA_iHL(void);
static void cpu_opcode_OR_rA_rA(void);

// Compare Register (or indirect HL) with A
static void cpu_opcode_CP_rA_rB(void);
static void cpu_opcode_CP_rA_rC(void);
static void cpu_opcode_CP_rA_rD(void);
static void cpu_opcode_CP_rA_rE(void);
static void cpu_opcode_CP_rA_rH(void);
static void cpu_opcode_CP_rA_rL(void);
static void cpu_opcode_CP_rA_iHL(void);
static void cpu_opcode_CP_rA_rA(void);

// Immediate ALU operation with A
static void cpu_opcode_ADD_rA_u8(void);
static void cpu_opcode_ADC_rA_u8(void);
static void cpu_opcode_SUB_rA_u8(void);
static void cpu_opcode_SBC_rA_u8(void);
static void cpu_opcode_AND_rA_u8(void);
static void cpu_opcode_XOR_rA_u8(void);
static void cpu_opcode_OR_rA_u8(void);
static void cpu_opcode_CP_rA_u8(void);

// Misc
static void cpu_opcode_DAA(void);
static void cpu_opcode_SCF(void);
static void cpu_opcode_CPL(void);
static void cpu_opcode_CCF(void);

// ----- 8-bit ALU -----

// ----- 16-bit ALU -----

// Increment Register
static inline void cpu_opcode_INC_rBC(void);
static inline void cpu_opcode_INC_rDE(void);
static inline void cpu_opcode_INC_rHL(void);
static inline void cpu_opcode_INC_rSP(void);

// Decrement Register
static void cpu_opcode_DEC_rBC(void);
static void cpu_opcode_DEC_rDE(void);
static void cpu_opcode_DEC_rHL(void);
static void cpu_opcode_DEC_rSP(void);

// Add Register to HL
static void cpu_opcode_ADD_rHL_rBC(void);
static void cpu_opcode_ADD_rHL_rDE(void);
static void cpu_opcode_ADD_rHL_rHL(void);
static void cpu_opcode_ADD_rHL_rSP(void);

// Misc
static void cpu_opcode_ADD_rSP_s8(void);
static void cpu_opcode_LDSP_rHL_s8(void);

// ----- 16-bit ALU -----

// ----- 8-bit Rotate/Shift -----

static void cpu_opcode_RLCA(void);
static void cpu_opcode_RRCA(void);
static void cpu_opcode_RLA(void);
static void cpu_opcode_RRA(void);

// ----- 8-bit Rotate/Shift -----

// ========== Standard Opcodes ==========

// ========== CB Prefix Opcodes ==========

// ----- 8-bit Rotate/Shift -----

// Rotate Left Circular
static void cpu_opcode_RLC_rB(void);
static void cpu_opcode_RLC_rC(void);
static void cpu_opcode_RLC_rD(void);
static void cpu_opcode_RLC_rE(void);
static void cpu_opcode_RLC_rH(void);
static void cpu_opcode_RLC_rL(void);
static void cpu_opcode_RLC_iHL(void);
static void cpu_opcode_RLC_rA(void);

// Rotate Right Circular
static void cpu_opcode_RRC_rB(void);
static void cpu_opcode_RRC_rC(void);
static void cpu_opcode_RRC_rD(void);
static void cpu_opcode_RRC_rE(void);
static void cpu_opcode_RRC_rH(void);
static void cpu_opcode_RRC_rL(void);
static void cpu_opcode_RRC_iHL(void);
static void cpu_opcode_RRC_rA(void);

// Rotate Left
static void cpu_opcode_RL_rB(void);
static void cpu_opcode_RL_rC(void);
static void cpu_opcode_RL_rD(void);
static void cpu_opcode_RL_rE(void);
static void cpu_opcode_RL_rH(void);
static void cpu_opcode_RL_rL(void);
static void cpu_opcode_RL_iHL(void);
static void cpu_opcode_RL_rA(void);

// Rotate Right
static void cpu_opcode_RR_rB(void);
static void cpu_opcode_RR_rC(void);
static void cpu_opcode_RR_rD(void);
static void cpu_opcode_RR_rE(void);
static void cpu_opcode_RR_rH(void);
static void cpu_opcode_RR_rL(void);
static void cpu_opcode_RR_iHL(void);
static void cpu_opcode_RR_rA(void);

// Shift Left Arithmetic
static void cpu_opcode_SLA_rB(void);
static void cpu_opcode_SLA_rC(void);
static void cpu_opcode_SLA_rD(void);
static void cpu_opcode_SLA_rE(void);
static void cpu_opcode_SLA_rH(void);
static void cpu_opcode_SLA_rL(void);
static void cpu_opcode_SLA_iHL(void);
static void cpu_opcode_SLA_rA(void);

// Shift Right Arithmetic
static void cpu_opcode_SRA_rB(void);
static void cpu_opcode_SRA_rC(void);
static void cpu_opcode_SRA_rD(void);
static void cpu_opcode_SRA_rE(void);
static void cpu_opcode_SRA_rH(void);
static void cpu_opcode_SRA_rL(void);
static void cpu_opcode_SRA_iHL(void);
static void cpu_opcode_SRA_rA(void);

// Swap A and another register (or indirect HL)
static void cpu_opcode_SWAP_rB(void);
static void cpu_opcode_SWAP_rC(void);
static void cpu_opcode_SWAP_rD(void);
static void cpu_opcode_SWAP_rE(void);
static void cpu_opcode_SWAP_rH(void);
static void cpu_opcode_SWAP_rL(void);
static void cpu_opcode_SWAP_iHL(void);
static void cpu_opcode_SWAP_rA(void);

// Shift Right Logical
static void cpu_opcode_SRL_rB(void);
static void cpu_opcode_SRL_rC(void);
static void cpu_opcode_SRL_rD(void);
static void cpu_opcode_SRL_rE(void);
static void cpu_opcode_SRL_rH(void);
static void cpu_opcode_SRL_rL(void);
static void cpu_opcode_SRL_iHL(void);
static void cpu_opcode_SRL_rA(void);

// ----- 8-bit Rotate/Shift -----

// ----- 8-bit Bit Operations -----

// Get bit
static void cpu_opcode_BIT_0_rB(void);
static void cpu_opcode_BIT_0_rC(void);
static void cpu_opcode_BIT_0_rD(void);
static void cpu_opcode_BIT_0_rE(void);
static void cpu_opcode_BIT_0_rH(void);
static void cpu_opcode_BIT_0_rL(void);
static void cpu_opcode_BIT_0_iHL(void);
static void cpu_opcode_BIT_0_rA(void);
static void cpu_opcode_BIT_1_rB(void);
static void cpu_opcode_BIT_1_rC(void);
static void cpu_opcode_BIT_1_rD(void);
static void cpu_opcode_BIT_1_rE(void);
static void cpu_opcode_BIT_1_rH(void);
static void cpu_opcode_BIT_1_rL(void);
static void cpu_opcode_BIT_1_iHL(void);
static void cpu_opcode_BIT_1_rA(void);
static void cpu_opcode_BIT_2_rB(void);
static void cpu_opcode_BIT_2_rC(void);
static void cpu_opcode_BIT_2_rD(void);
static void cpu_opcode_BIT_2_rE(void);
static void cpu_opcode_BIT_2_rH(void);
static void cpu_opcode_BIT_2_rL(void);
static void cpu_opcode_BIT_2_iHL(void);
static void cpu_opcode_BIT_2_rA(void);
static void cpu_opcode_BIT_3_rB(void);
static void cpu_opcode_BIT_3_rC(void);
static void cpu_opcode_BIT_3_rD(void);
static void cpu_opcode_BIT_3_rE(void);
static void cpu_opcode_BIT_3_rH(void);
static void cpu_opcode_BIT_3_rL(void);
static void cpu_opcode_BIT_3_iHL(void);
static void cpu_opcode_BIT_3_rA(void);
static void cpu_opcode_BIT_4_rB(void);
static void cpu_opcode_BIT_4_rC(void);
static void cpu_opcode_BIT_4_rD(void);
static void cpu_opcode_BIT_4_rE(void);
static void cpu_opcode_BIT_4_rH(void);
static void cpu_opcode_BIT_4_rL(void);
static void cpu_opcode_BIT_4_iHL(void);
static void cpu_opcode_BIT_4_rA(void);
static void cpu_opcode_BIT_5_rB(void);
static void cpu_opcode_BIT_5_rC(void);
static void cpu_opcode_BIT_5_rD(void);
static void cpu_opcode_BIT_5_rE(void);
static void cpu_opcode_BIT_5_rH(void);
static void cpu_opcode_BIT_5_rL(void);
static void cpu_opcode_BIT_5_iHL(void);
static void cpu_opcode_BIT_5_rA(void);
static void cpu_opcode_BIT_6_rB(void);
static void cpu_opcode_BIT_6_rC(void);
static void cpu_opcode_BIT_6_rD(void);
static void cpu_opcode_BIT_6_rE(void);
static void cpu_opcode_BIT_6_rH(void);
static void cpu_opcode_BIT_6_rL(void);
static void cpu_opcode_BIT_6_iHL(void);
static void cpu_opcode_BIT_6_rA(void);
static void cpu_opcode_BIT_7_rB(void);
static void cpu_opcode_BIT_7_rC(void);
static void cpu_opcode_BIT_7_rD(void);
static void cpu_opcode_BIT_7_rE(void);
static void cpu_opcode_BIT_7_rH(void);
static void cpu_opcode_BIT_7_rL(void);
static void cpu_opcode_BIT_7_iHL(void);
static void cpu_opcode_BIT_7_rA(void);

// Reset bit
static void cpu_opcode_RES_0_rB(void);
static void cpu_opcode_RES_0_rC(void);
static void cpu_opcode_RES_0_rD(void);
static void cpu_opcode_RES_0_rE(void);
static void cpu_opcode_RES_0_rH(void);
static void cpu_opcode_RES_0_rL(void);
static void cpu_opcode_RES_0_iHL(void);
static void cpu_opcode_RES_0_rA(void);
static void cpu_opcode_RES_1_rB(void);
static void cpu_opcode_RES_1_rC(void);
static void cpu_opcode_RES_1_rD(void);
static void cpu_opcode_RES_1_rE(void);
static void cpu_opcode_RES_1_rH(void);
static void cpu_opcode_RES_1_rL(void);
static void cpu_opcode_RES_1_iHL(void);
static void cpu_opcode_RES_1_rA(void);
static void cpu_opcode_RES_2_rB(void);
static void cpu_opcode_RES_2_rC(void);
static void cpu_opcode_RES_2_rD(void);
static void cpu_opcode_RES_2_rE(void);
static void cpu_opcode_RES_2_rH(void);
static void cpu_opcode_RES_2_rL(void);
static void cpu_opcode_RES_2_iHL(void);
static void cpu_opcode_RES_2_rA(void);
static void cpu_opcode_RES_3_rB(void);
static void cpu_opcode_RES_3_rC(void);
static void cpu_opcode_RES_3_rD(void);
static void cpu_opcode_RES_3_rE(void);
static void cpu_opcode_RES_3_rH(void);
static void cpu_opcode_RES_3_rL(void);
static void cpu_opcode_RES_3_iHL(void);
static void cpu_opcode_RES_3_rA(void);
static void cpu_opcode_RES_4_rB(void);
static void cpu_opcode_RES_4_rC(void);
static void cpu_opcode_RES_4_rD(void);
static void cpu_opcode_RES_4_rE(void);
static void cpu_opcode_RES_4_rH(void);
static void cpu_opcode_RES_4_rL(void);
static void cpu_opcode_RES_4_iHL(void);
static void cpu_opcode_RES_4_rA(void);
static void cpu_opcode_RES_5_rB(void);
static void cpu_opcode_RES_5_rC(void);
static void cpu_opcode_RES_5_rD(void);
static void cpu_opcode_RES_5_rE(void);
static void cpu_opcode_RES_5_rH(void);
static void cpu_opcode_RES_5_rL(void);
static void cpu_opcode_RES_5_iHL(void);
static void cpu_opcode_RES_5_rA(void);
static void cpu_opcode_RES_6_rB(void);
static void cpu_opcode_RES_6_rC(void);
static void cpu_opcode_RES_6_rD(void);
static void cpu_opcode_RES_6_rE(void);
static void cpu_opcode_RES_6_rH(void);
static void cpu_opcode_RES_6_rL(void);
static void cpu_opcode_RES_6_iHL(void);
static void cpu_opcode_RES_6_rA(void);
static void cpu_opcode_RES_7_rB(void);
static void cpu_opcode_RES_7_rC(void);
static void cpu_opcode_RES_7_rD(void);
static void cpu_opcode_RES_7_rE(void);
static void cpu_opcode_RES_7_rH(void);
static void cpu_opcode_RES_7_rL(void);
static void cpu_opcode_RES_7_iHL(void);
static void cpu_opcode_RES_7_rA(void);

// Set bit
static void cpu_opcode_SET_0_rB(void);
static void cpu_opcode_SET_0_rC(void);
static void cpu_opcode_SET_0_rD(void);
static void cpu_opcode_SET_0_rE(void);
static void cpu_opcode_SET_0_rH(void);
static void cpu_opcode_SET_0_rL(void);
static void cpu_opcode_SET_0_iHL(void);
static void cpu_opcode_SET_0_rA(void);
static void cpu_opcode_SET_1_rB(void);
static void cpu_opcode_SET_1_rC(void);
static void cpu_opcode_SET_1_rD(void);
static void cpu_opcode_SET_1_rE(void);
static void cpu_opcode_SET_1_rH(void);
static void cpu_opcode_SET_1_rL(void);
static void cpu_opcode_SET_1_iHL(void);
static void cpu_opcode_SET_1_rA(void);
static void cpu_opcode_SET_2_rB(void);
static void cpu_opcode_SET_2_rC(void);
static void cpu_opcode_SET_2_rD(void);
static void cpu_opcode_SET_2_rE(void);
static void cpu_opcode_SET_2_rH(void);
static void cpu_opcode_SET_2_rL(void);
static void cpu_opcode_SET_2_iHL(void);
static void cpu_opcode_SET_2_rA(void);
static void cpu_opcode_SET_3_rB(void);
static void cpu_opcode_SET_3_rC(void);
static void cpu_opcode_SET_3_rD(void);
static void cpu_opcode_SET_3_rE(void);
static void cpu_opcode_SET_3_rH(void);
static void cpu_opcode_SET_3_rL(void);
static void cpu_opcode_SET_3_iHL(void);
static void cpu_opcode_SET_3_rA(void);
static void cpu_opcode_SET_4_rB(void);
static void cpu_opcode_SET_4_rC(void);
static void cpu_opcode_SET_4_rD(void);
static void cpu_opcode_SET_4_rE(void);
static void cpu_opcode_SET_4_rH(void);
static void cpu_opcode_SET_4_rL(void);
static void cpu_opcode_SET_4_iHL(void);
static void cpu_opcode_SET_4_rA(void);
static void cpu_opcode_SET_5_rB(void);
static void cpu_opcode_SET_5_rC(void);
static void cpu_opcode_SET_5_rD(void);
static void cpu_opcode_SET_5_rE(void);
static void cpu_opcode_SET_5_rH(void);
static void cpu_opcode_SET_5_rL(void);
static void cpu_opcode_SET_5_iHL(void);
static void cpu_opcode_SET_5_rA(void);
static void cpu_opcode_SET_6_rB(void);
static void cpu_opcode_SET_6_rC(void);
static void cpu_opcode_SET_6_rD(void);
static void cpu_opcode_SET_6_rE(void);
static void cpu_opcode_SET_6_rH(void);
static void cpu_opcode_SET_6_rL(void);
static void cpu_opcode_SET_6_iHL(void);
static void cpu_opcode_SET_6_rA(void);
static void cpu_opcode_SET_7_rB(void);
static void cpu_opcode_SET_7_rC(void);
static void cpu_opcode_SET_7_rD(void);
static void cpu_opcode_SET_7_rE(void);
static void cpu_opcode_SET_7_rH(void);
static void cpu_opcode_SET_7_rL(void);
static void cpu_opcode_SET_7_iHL(void);
static void cpu_opcode_SET_7_rA(void);

// ----- 8-bit Bit Operations -----

// ========== CB Prefix Opcodes ==========
