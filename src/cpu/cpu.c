/**
 * @file cpu.c
 * @brief Implementation file for the Gameboy's CPU.
 */
#include "cpu/cpu.h"

#include <log.h>
#include <string.h>

#include "DEFINES.h"
#include "cpu/alu.h"
#include "cpu/registers.h"
#include "memory/mmu.h"

LOG_MODULE_SETUP("CPU", CONFIG_CPU_MODULE_LOG_LEVEL)

typedef void (*CPU_INSTR)(void);

static int              m_status = 0;
static struct registers m_reg    = {0};

static byte cpu_read_u8(void);
static word cpu_read_u16(void);

// Dump Opcode decls here
// This is done to cut down on bloat in this file as the opcodes are 1000+ lines on their own
#include "cpu/cpu_opcodes.inc"

// clang-format off
static const CPU_INSTR m_opcode_table[0x100] = {
        // 0x_0                | 0x_1                 | 0x_2                 | 0x_3                | 0x_4                  | 0x_5                | 0x_6                 | 0x_7                | 0x_8                | 0x_9                | 0x_A                 | 0x_B                | 0x_C                 | 0x_D                | 0x_E                 | 0x_F
/* 0x0_ */ cpu_opcode_NOP,       cpu_opcode_LD_rBC_u16, cpu_opcode_LD_iBC_rA,  cpu_opcode_INC_rBC,   cpu_opcode_INC_rB,      cpu_opcode_DEC_rB,    cpu_opcode_LD_rB_u8,   cpu_opcode_RLCA,      cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_LD_rA_iBC,  cpu_opcode_undefined, cpu_opcode_INC_rC,     cpu_opcode_DEC_rC,    cpu_opcode_LD_rC_u8,   cpu_opcode_RRCA,
/* 0x1_ */ cpu_opcode_undefined, cpu_opcode_LD_rDE_u16, cpu_opcode_LD_iDE_rA,  cpu_opcode_INC_rDE,   cpu_opcode_INC_rD,      cpu_opcode_DEC_rD,    cpu_opcode_LD_rD_u8,   cpu_opcode_RLA,       cpu_opcode_JR_s8,     cpu_opcode_undefined, cpu_opcode_LD_rA_iDE,  cpu_opcode_undefined, cpu_opcode_INC_rE,     cpu_opcode_DEC_rE,    cpu_opcode_LD_rE_u8,   cpu_opcode_RRA,
/* 0x2_ */ cpu_opcode_JR_NZ_s8,  cpu_opcode_LD_rHL_u16, cpu_opcode_LD_iHLi_rA, cpu_opcode_INC_rHL,   cpu_opcode_INC_rH,      cpu_opcode_DEC_rH,    cpu_opcode_LD_rH_u8,   cpu_opcode_undefined, cpu_opcode_JR_Z_s8,   cpu_opcode_undefined, cpu_opcode_LD_rA_iHLi, cpu_opcode_undefined, cpu_opcode_INC_rL,     cpu_opcode_DEC_rL,    cpu_opcode_LD_rL_u8,   cpu_opcode_undefined,
/* 0x3_ */ cpu_opcode_JR_NC_s8,  cpu_opcode_LD_rSP_u16, cpu_opcode_LD_iHLd_rA, cpu_opcode_INC_rSP,   cpu_opcode_INC_iHL,     cpu_opcode_DEC_iHL,   cpu_opcode_LD_iHL_u8,  cpu_opcode_undefined, cpu_opcode_JR_C_s8,   cpu_opcode_undefined, cpu_opcode_LD_rA_iHLd, cpu_opcode_undefined, cpu_opcode_INC_rA,     cpu_opcode_DEC_rA,    cpu_opcode_LD_rA_u8,   cpu_opcode_undefined,
/* 0x4_ */ cpu_opcode_LD_rB_rB,  cpu_opcode_LD_rB_rC,   cpu_opcode_LD_rB_rD,   cpu_opcode_LD_rB_rE,  cpu_opcode_LD_rB_rH,    cpu_opcode_LD_rB_rL,  cpu_opcode_LD_rB_iHL,  cpu_opcode_LD_rB_rA,  cpu_opcode_LD_rC_rB,  cpu_opcode_LD_rC_rC,  cpu_opcode_LD_rC_rD,   cpu_opcode_LD_rC_rE,  cpu_opcode_LD_rC_rH,   cpu_opcode_LD_rC_rL,  cpu_opcode_LD_rC_iHL,  cpu_opcode_LD_rC_rA, 
/* 0x5_ */ cpu_opcode_LD_rD_rB,  cpu_opcode_LD_rD_rC,   cpu_opcode_LD_rD_rD,   cpu_opcode_LD_rD_rE,  cpu_opcode_LD_rD_rH,    cpu_opcode_LD_rD_rL,  cpu_opcode_LD_rD_iHL,  cpu_opcode_LD_rD_rA,  cpu_opcode_LD_rE_rB,  cpu_opcode_LD_rE_rC,  cpu_opcode_LD_rE_rD,   cpu_opcode_LD_rE_rE,  cpu_opcode_LD_rE_rH,   cpu_opcode_LD_rE_rL,  cpu_opcode_LD_rE_iHL,  cpu_opcode_LD_rE_rA, 
/* 0x6_ */ cpu_opcode_LD_rH_rB,  cpu_opcode_LD_rH_rC,   cpu_opcode_LD_rH_rD,   cpu_opcode_LD_rH_rE,  cpu_opcode_LD_rH_rH,    cpu_opcode_LD_rH_rL,  cpu_opcode_LD_rH_iHL,  cpu_opcode_LD_rH_rA,  cpu_opcode_LD_rL_rB,  cpu_opcode_LD_rL_rC,  cpu_opcode_LD_rL_rD,   cpu_opcode_LD_rL_rE,  cpu_opcode_LD_rL_rH,   cpu_opcode_LD_rL_rL,  cpu_opcode_LD_rL_iHL,  cpu_opcode_LD_rL_rA, 
/* 0x7_ */ cpu_opcode_LD_iHL_rB, cpu_opcode_LD_iHL_rC,  cpu_opcode_LD_iHL_rD,  cpu_opcode_LD_iHL_rE, cpu_opcode_LD_iHL_rH,   cpu_opcode_LD_iHL_rL, cpu_opcode_undefined,  cpu_opcode_LD_iHL_rA, cpu_opcode_LD_rA_rB,  cpu_opcode_LD_rA_rC,  cpu_opcode_LD_rA_rD,   cpu_opcode_LD_rA_rE,  cpu_opcode_LD_rA_rH,   cpu_opcode_LD_rA_rL,  cpu_opcode_LD_rA_iHL,  cpu_opcode_LD_rA_rA,
/* 0x8_ */ cpu_opcode_ADD_rA_rB, cpu_opcode_ADD_rA_rC,  cpu_opcode_ADD_rA_rD,  cpu_opcode_ADD_rA_rE, cpu_opcode_ADD_rA_rH,   cpu_opcode_ADD_rA_rL, cpu_opcode_ADD_rA_iHL, cpu_opcode_ADD_rA_rA, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined,
/* 0x9_ */ cpu_opcode_SUB_rA_rB, cpu_opcode_SUB_rA_rC,  cpu_opcode_SUB_rA_rD,  cpu_opcode_SUB_rA_rE, cpu_opcode_SUB_rA_rH,   cpu_opcode_SUB_rA_rL, cpu_opcode_SUB_rA_iHL, cpu_opcode_SUB_rA_rA, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined,
/* 0xA_ */ cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined,   cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_XOR_rA_rB, cpu_opcode_XOR_rA_rC, cpu_opcode_XOR_rA_rD,  cpu_opcode_XOR_rA_rE, cpu_opcode_XOR_rA_rH,  cpu_opcode_XOR_rA_rL, cpu_opcode_XOR_rA_iHL, cpu_opcode_XOR_rA_rA,
/* 0xB_ */ cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined,   cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_CP_rA_rB,  cpu_opcode_CP_rA_rC,  cpu_opcode_CP_rA_rD,   cpu_opcode_CP_rA_rE,  cpu_opcode_CP_rA_rH,   cpu_opcode_CP_rA_rL,  cpu_opcode_CP_rA_iHL,  cpu_opcode_CP_rA_rA,
/* 0xC_ */ cpu_opcode_RET_NZ,    cpu_opcode_POP_rBC,    cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_CALL_NZ_u16, cpu_opcode_PUSH_rBC,  cpu_opcode_ADD_rA_u8,  cpu_opcode_undefined, cpu_opcode_RET_Z,     cpu_opcode_RET,       cpu_opcode_undefined,  cpu_opcode_CB,        cpu_opcode_CALL_Z_u16, cpu_opcode_CALL_u16,  cpu_opcode_undefined,  cpu_opcode_undefined,
/* 0xD_ */ cpu_opcode_RET_NC,    cpu_opcode_POP_rDE,    cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_CALL_NC_u16, cpu_opcode_PUSH_rDE,  cpu_opcode_SUB_rA_u8,  cpu_opcode_undefined, cpu_opcode_RET_C,     cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_CALL_C_u16, cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined,
/* 0xE_ */ cpu_opcode_LDH_u8_rA, cpu_opcode_POP_rHL,    cpu_opcode_LDH_rC_rA,  cpu_opcode_undefined, cpu_opcode_undefined,   cpu_opcode_PUSH_rHL,  cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_LD_i16_rA,  cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_XOR_rA_u8,  cpu_opcode_undefined,
/* 0xF_ */ cpu_opcode_LDH_rA_u8, cpu_opcode_POP_rAF,    cpu_opcode_LDH_rA_rC,  cpu_opcode_undefined, cpu_opcode_undefined,   cpu_opcode_PUSH_rAF,  cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_LD_rA_i16,  cpu_opcode_undefined, cpu_opcode_undefined,  cpu_opcode_undefined, cpu_opcode_CP_rA_u8,   cpu_opcode_undefined
};

static const CPU_INSTR m_cb_opcode_table[0x100] = {
        // 0x_0                | 0x_1                | 0x_2                | 0x_3                | 0x_4                | 0x_5                | 0x_6                | 0x_7                | 0x_8                | 0x_9                | 0x_A                | 0x_B                | 0x_C                | 0x_D                | 0x_E                | 0x_F
/* 0x0_ */ cpu_opcode_RLC_rB,    cpu_opcode_RLC_rC,    cpu_opcode_RLC_rD,    cpu_opcode_RLC_rE,    cpu_opcode_RLC_rH,    cpu_opcode_RLC_rL,    cpu_opcode_RLC_iHL,   cpu_opcode_RLC_rA,    cpu_opcode_RRC_rB,    cpu_opcode_RRC_rC,    cpu_opcode_RRC_rD,    cpu_opcode_RRC_rE,    cpu_opcode_RRC_rH,    cpu_opcode_RRC_rL,    cpu_opcode_RRC_iHL,   cpu_opcode_RRC_rA,
/* 0x1_ */ cpu_opcode_RL_rB,     cpu_opcode_RL_rC,     cpu_opcode_RL_rD,     cpu_opcode_RL_rE,     cpu_opcode_RL_rH,     cpu_opcode_RL_rL,     cpu_opcode_RL_iHL,    cpu_opcode_RL_rA,     cpu_opcode_RR_rB,     cpu_opcode_RR_rC,     cpu_opcode_RR_rD,     cpu_opcode_RR_rE,     cpu_opcode_RR_rH,     cpu_opcode_RR_rL,     cpu_opcode_RR_iHL,    cpu_opcode_RR_rA,
/* 0x2_ */ cpu_opcode_SLA_rB,    cpu_opcode_SLA_rC,    cpu_opcode_SLA_rD,    cpu_opcode_SLA_rE,    cpu_opcode_SLA_rH,    cpu_opcode_SLA_rL,    cpu_opcode_SLA_iHL,   cpu_opcode_SLA_rA,    cpu_opcode_SRA_rB,    cpu_opcode_SRA_rC,    cpu_opcode_SRA_rD,    cpu_opcode_SRA_rE,    cpu_opcode_SRA_rH,    cpu_opcode_SRA_rL,    cpu_opcode_SRA_iHL,   cpu_opcode_SRA_rA,
/* 0x3_ */ cpu_opcode_SWAP_rB,   cpu_opcode_SWAP_rC,   cpu_opcode_SWAP_rD,   cpu_opcode_SWAP_rE,   cpu_opcode_SWAP_rH,   cpu_opcode_SWAP_rL,   cpu_opcode_SWAP_iHL,  cpu_opcode_SWAP_rA,   cpu_opcode_SLA_rB,    cpu_opcode_SLA_rC,    cpu_opcode_SLA_rD,    cpu_opcode_SLA_rE,    cpu_opcode_SLA_rH,    cpu_opcode_SLA_rL,    cpu_opcode_SLA_iHL,   cpu_opcode_SLA_rA,
/* 0x4_ */ cpu_opcode_BIT_0_rB,  cpu_opcode_BIT_0_rC,  cpu_opcode_BIT_0_rD,  cpu_opcode_BIT_0_rE,  cpu_opcode_BIT_0_rH,  cpu_opcode_BIT_0_rL,  cpu_opcode_BIT_0_iHL, cpu_opcode_BIT_0_rA,  cpu_opcode_BIT_1_rB,  cpu_opcode_BIT_1_rC,  cpu_opcode_BIT_1_rD,  cpu_opcode_BIT_1_rE,  cpu_opcode_BIT_1_rH,  cpu_opcode_BIT_1_rL,  cpu_opcode_BIT_1_iHL, cpu_opcode_BIT_1_rA,
/* 0x5_ */ cpu_opcode_BIT_2_rB,  cpu_opcode_BIT_2_rC,  cpu_opcode_BIT_2_rD,  cpu_opcode_BIT_2_rE,  cpu_opcode_BIT_2_rH,  cpu_opcode_BIT_2_rL,  cpu_opcode_BIT_2_iHL, cpu_opcode_BIT_2_rA,  cpu_opcode_BIT_3_rB,  cpu_opcode_BIT_3_rC,  cpu_opcode_BIT_3_rD,  cpu_opcode_BIT_3_rE,  cpu_opcode_BIT_3_rH,  cpu_opcode_BIT_3_rL,  cpu_opcode_BIT_3_iHL, cpu_opcode_BIT_3_rA,
/* 0x6_ */ cpu_opcode_BIT_4_rB,  cpu_opcode_BIT_4_rC,  cpu_opcode_BIT_4_rD,  cpu_opcode_BIT_4_rE,  cpu_opcode_BIT_4_rH,  cpu_opcode_BIT_4_rL,  cpu_opcode_BIT_4_iHL, cpu_opcode_BIT_4_rA,  cpu_opcode_BIT_5_rB,  cpu_opcode_BIT_5_rC,  cpu_opcode_BIT_5_rD,  cpu_opcode_BIT_5_rE,  cpu_opcode_BIT_5_rH,  cpu_opcode_BIT_5_rL,  cpu_opcode_BIT_5_iHL, cpu_opcode_BIT_5_rA,
/* 0x7_ */ cpu_opcode_BIT_6_rB,  cpu_opcode_BIT_6_rC,  cpu_opcode_BIT_6_rD,  cpu_opcode_BIT_6_rE,  cpu_opcode_BIT_6_rH,  cpu_opcode_BIT_6_rL,  cpu_opcode_BIT_6_iHL, cpu_opcode_BIT_6_rA,  cpu_opcode_BIT_7_rB,  cpu_opcode_BIT_7_rC,  cpu_opcode_BIT_7_rD,  cpu_opcode_BIT_7_rE,  cpu_opcode_BIT_7_rH,  cpu_opcode_BIT_7_rL,  cpu_opcode_BIT_7_iHL, cpu_opcode_BIT_7_rA,
/* 0x8_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,
/* 0x9_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,
/* 0xA_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,
/* 0xB_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,
/* 0xC_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,
/* 0xD_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,
/* 0xE_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined,
/* 0xF_ */ cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined, cpu_opcode_undefined
};
// clang-format on

int cpu_execute(void) {
    // Get Opcode
    // Fetch function pointer
    // Exec function
    // Return status (functions can change it)
    byte      opcode = cpu_read_u8();
    CPU_INSTR instr  = m_opcode_table[opcode];
    instr();

    // if (m_reg.PC == 0x0099) {
    //     log_warn(
    //         "\n"
    //         "=== CPU Breakpoint Hit ===\n"
    //         "       ADDR: 0x%.4X\n"
    //         "==========================",
    //         m_reg.PC
    //     );
    //     return -1;
    // }

    return m_status;
}

struct registers cpu_snapshot_registers(void) { return m_reg; }

static inline byte cpu_read_u8(void) { return mmu_read(BUS_CPU, m_reg.PC++); }
static inline word cpu_read_u16(void) {
    byte lo = cpu_read_u8();
    byte hi = cpu_read_u8();
    return (((word)hi) << 8) + lo;
};

static inline void cpu_push_u8(const byte value) { mmu_write(BUS_CPU, --m_reg.SP, value); }
static inline void cpu_push_u16(const word value) {
    byte lo = (byte)(value & 0x00FF);
    byte hi = (byte)((value & 0xFF00) >> 8);

    cpu_push_u8(hi);
    cpu_push_u8(lo);
}
static inline byte cpu_pop_u8(void) { return mmu_read(BUS_CPU, m_reg.SP++); }
static inline word cpu_pop_u16(void) {
    byte lo = cpu_pop_u8();
    byte hi = cpu_pop_u8();

    return (((word)hi) << 8) + lo;
}

static void cpu_opcode_undefined(void) {
    word opcode_addr = m_reg.PC - 1;
    if (mmu_read(BUS_CPU, m_reg.PC - 2) == 0xCB) {
        log_warn(
            "Undefined CB opcode reached:\n\tADDR: 0x%.4X\n\tOpcode: 0xCB%.2X",
            opcode_addr - 1,
            mmu_read(BUS_CPU, opcode_addr)
        );
    } else {
        log_warn(
            "Undefined opcode reached:\n\tADDR: 0x%.4X\n\tOpcode: 0x%.2X",
            opcode_addr,
            mmu_read(BUS_CPU, opcode_addr)
        );
    }
    m_status = 1;
};

static void cpu_opcode_illegal(void) {
    word opcode_addr = m_reg.PC - 1;
    log_error(
        "Illegal opcode reached:\n\tADDR: 0x%.4X\n\tOpcode: 0x%.2X",
        opcode_addr,
        mmu_read(BUS_CPU, opcode_addr)
    );
    m_status = -1;
}

// ========== Generic Opcodes ==========
static inline void cpu_generic_JR_cond_i8(const bool condition) {
    byte   offset_unsigned = cpu_read_u8();
    s_byte offset_signed   = 0x00;
    memcpy(&offset_signed, &offset_unsigned, sizeof(byte));

    if (condition == true) { m_reg.PC += offset_signed; }
}

static inline void cpu_generic_RET_cond(const bool condition) {
    if (condition == true) { m_reg.PC = cpu_pop_u16(); }
}

static inline void cpu_generic_CALL_cond_i16(const bool condition) {
    word call_address = cpu_read_u16();

    if (condition == true) {
        cpu_push_u16(m_reg.PC);
        m_reg.PC = call_address;
    }
}

static inline void cpu_generic_LD_r8_u8(byte* dst, const byte src) { *dst = src; }
static inline void cpu_generic_LD_ir16_u8(const word dst_indirect, const byte src) {
    mmu_write(BUS_CPU, dst_indirect, src);
}
static inline void cpu_generic_LD_r8_ir16(byte* dst, const word src_indirect) {
    *dst = mmu_read(BUS_CPU, src_indirect);
}

static inline void cpu_generic_LD_r16_u16(word* dst, const word src) { *dst = src; }

static inline void cpu_generic_LDH_u8_r8(const byte offset, const byte src) {
    mmu_write(BUS_CPU, 0xFF00 + (word)offset, src);
}
static inline void cpu_generic_LDH_r8_u8(byte* dst, const byte offset) {
    *dst = mmu_read(BUS_CPU, 0xFF00 + (word)offset);
}

static inline void cpu_generic_PUSH_r16(const word src) { cpu_push_u16(src); }
static inline void cpu_generic_POP_r16(word* dst) { *dst = cpu_pop_u16(); }

static inline void cpu_generic_INC_r8(byte* dst) {
    struct alu_result result = alu_inc_8b(*dst);
    *dst                     = result.result.byte;
    registers_set_flags(
        &m_reg,
        result.flags.z,
        false,
        result.flags.h,
        registers_get_flag_c(&m_reg)
    );
}
static inline void cpu_generic_DEC_r8(byte* dst) {
    struct alu_result result = alu_dec_8b(*dst);
    *dst                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, true, result.flags.h, registers_get_flag_c(&m_reg));
}

static inline void cpu_generic_ADD_r8_u8(byte* dst, const byte src) {
    struct alu_result result = alu_add_8b(*dst, src);
    *dst                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, result.flags.h, result.flags.c);
}

static inline void cpu_generic_SUB_r8_u8(byte* dst, const byte src) {
    struct alu_result result = alu_sub_8b(*dst, src);
    *dst                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, true, result.flags.h, result.flags.c);
}

static inline void cpu_generic_XOR_r8_u8(byte* dst, const byte src) {
    struct alu_result result = alu_xor_8b(*dst, src);
    *dst                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, false);
}

static inline void cpu_generic_CP_r8_u8(byte* dst, const byte src) {
    struct alu_result result = alu_cp_8b(*dst, src);
    *dst                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, true, result.flags.h, result.flags.c);
}

static inline void cpu_generic_INC_r16(word* reg) { (*reg)++; }

static inline void cpu_generic_RLC_r8(byte* reg) {
    struct alu_result result = alu_rlc_8b(*reg);
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, result.flags.c);
}
static inline void cpu_generic_RRC_r8(byte* reg) {
    struct alu_result result = alu_rrc_8b(*reg);
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, result.flags.c);
}
static inline void cpu_generic_RL_r8(byte* reg) {
    struct alu_result result = alu_rl_8b(*reg, registers_get_flag_c(&m_reg));
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, result.flags.c);
}
static inline void cpu_generic_RR_r8(byte* reg) {
    struct alu_result result = alu_rr_8b(*reg, registers_get_flag_c(&m_reg));
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, result.flags.c);
}
static inline void cpu_generic_SLA_r8(byte* reg) {
    struct alu_result result = alu_sla_8b(*reg);
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, result.flags.c);
}
static inline void cpu_generic_SRA_r8(byte* reg) {
    struct alu_result result = alu_sra_8b(*reg);
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, result.flags.c);
}
static inline void cpu_generic_SWAP_r8(byte* reg) {
    struct alu_result result = alu_swap_8b(*reg);
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, false);
}
static inline void cpu_generic_SRL_r8(byte* reg) {
    struct alu_result result = alu_srl_8b(*reg);
    *reg                     = result.result.byte;
    registers_set_flags(&m_reg, result.flags.z, false, false, result.flags.c);
}

static inline void cpu_generic_BIT(const size_t bit, const byte trg) {
    struct alu_result result = alu_bit_8b(bit, trg);
    registers_set_flags(&m_reg, result.flags.z, false, true, registers_get_flag_c(&m_reg));
}
// ========== Generic Opcodes ==========

// ========== Standard Opcodes ==========

// ----- Control -----

// Misc
static void cpu_opcode_NOP(void) {
    log_debug("Executed NOP");
    return;
}
static void cpu_opcode_STOP(void);
static void cpu_opcode_HALT(void);
static void cpu_opcode_CB(void) { m_cb_opcode_table[cpu_read_u8()](); }
static void cpu_opcode_DI(void);
static void cpu_opcode_EI(void);
// Misc

// Branch
static void cpu_opcode_JR_s8(void) { cpu_generic_JR_cond_i8(true); }
static void cpu_opcode_JR_NZ_s8(void) {
    cpu_generic_JR_cond_i8(registers_get_flag_z(&m_reg) == false);
}
static void cpu_opcode_JR_NC_s8(void) {
    cpu_generic_JR_cond_i8(registers_get_flag_c(&m_reg) == false);
}
static void cpu_opcode_JR_Z_s8(void) {
    cpu_generic_JR_cond_i8(registers_get_flag_z(&m_reg) == true);
}
static void cpu_opcode_JR_C_s8(void) {
    cpu_generic_JR_cond_i8(registers_get_flag_c(&m_reg) == true);
}
static void cpu_opcode_RET(void) { cpu_generic_RET_cond(true); }
static void cpu_opcode_RET_NZ(void) { cpu_generic_RET_cond(registers_get_flag_z(&m_reg) == false); }
static void cpu_opcode_RET_NC(void) { cpu_generic_RET_cond(registers_get_flag_c(&m_reg) == false); }
static void cpu_opcode_RET_Z(void) { cpu_generic_RET_cond(registers_get_flag_z(&m_reg) == true); }
static void cpu_opcode_RET_C(void) { cpu_generic_RET_cond(registers_get_flag_c(&m_reg) == true); }
static void cpu_opcode_RETI(void);
static void cpu_opcode_JP_u16(void);
static void cpu_opcode_JP_NZ_u16(void);
static void cpu_opcode_JP_NC_u16(void);
static void cpu_opcode_JP_Z_u16(void);
static void cpu_opcode_JP_C_u16(void);
static void cpu_opcode_JP_rHL(void);
static void cpu_opcode_CALL_u16(void) { cpu_generic_CALL_cond_i16(true); }
static void cpu_opcode_CALL_NZ_u16(void) {
    cpu_generic_CALL_cond_i16(registers_get_flag_z(&m_reg) == false);
}
static void cpu_opcode_CALL_NC_u16(void) {
    cpu_generic_CALL_cond_i16(registers_get_flag_c(&m_reg) == false);
}
static void cpu_opcode_CALL_Z_u16(void) {
    cpu_generic_CALL_cond_i16(registers_get_flag_z(&m_reg) == true);
}
static void cpu_opcode_CALL_C_u16(void) {
    cpu_generic_CALL_cond_i16(registers_get_flag_c(&m_reg) == true);
}
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
static void cpu_opcode_LD_rB_u8(void) { cpu_generic_LD_r8_u8(&m_reg.B, cpu_read_u8()); }
static void cpu_opcode_LD_rC_u8(void) { cpu_generic_LD_r8_u8(&m_reg.C, cpu_read_u8()); }
static void cpu_opcode_LD_rD_u8(void) { cpu_generic_LD_r8_u8(&m_reg.D, cpu_read_u8()); }
static void cpu_opcode_LD_rE_u8(void) { cpu_generic_LD_r8_u8(&m_reg.E, cpu_read_u8()); }
static void cpu_opcode_LD_rH_u8(void) { cpu_generic_LD_r8_u8(&m_reg.H, cpu_read_u8()); }
static void cpu_opcode_LD_rL_u8(void) { cpu_generic_LD_r8_u8(&m_reg.L, cpu_read_u8()); }
static void cpu_opcode_LD_iHL_u8(void) { cpu_generic_LD_ir16_u8(m_reg.HL, cpu_read_u8()); }
static void cpu_opcode_LD_rA_u8(void) { cpu_generic_LD_r8_u8(&m_reg.A, cpu_read_u8()); }

// Load indirectly
static void cpu_opcode_LD_iBC_rA(void) { cpu_generic_LD_ir16_u8(m_reg.BC, m_reg.A); }
static void cpu_opcode_LD_iDE_rA(void) { cpu_generic_LD_ir16_u8(m_reg.DE, m_reg.A); }
static void cpu_opcode_LD_iHLi_rA(void) { cpu_generic_LD_ir16_u8(m_reg.HL++, m_reg.A); }
static void cpu_opcode_LD_iHLd_rA(void) { cpu_generic_LD_ir16_u8(m_reg.HL--, m_reg.A); }
static void cpu_opcode_LD_rA_iBC(void) { cpu_generic_LD_r8_ir16(&m_reg.A, m_reg.BC); }
static void cpu_opcode_LD_rA_iDE(void) { cpu_generic_LD_r8_ir16(&m_reg.A, m_reg.DE); }
static void cpu_opcode_LD_rA_iHLi(void) { cpu_generic_LD_r8_ir16(&m_reg.A, m_reg.HL++); }
static void cpu_opcode_LD_rA_iHLd(void) { cpu_generic_LD_r8_ir16(&m_reg.A, m_reg.HL--); }

// Load B from Register (or indirect HL)
static void cpu_opcode_LD_rB_rB(void) { cpu_generic_LD_r8_u8(&m_reg.B, m_reg.B); }
static void cpu_opcode_LD_rB_rC(void) { cpu_generic_LD_r8_u8(&m_reg.B, m_reg.C); }
static void cpu_opcode_LD_rB_rD(void) { cpu_generic_LD_r8_u8(&m_reg.B, m_reg.D); }
static void cpu_opcode_LD_rB_rE(void) { cpu_generic_LD_r8_u8(&m_reg.B, m_reg.E); }
static void cpu_opcode_LD_rB_rH(void) { cpu_generic_LD_r8_u8(&m_reg.B, m_reg.H); }
static void cpu_opcode_LD_rB_rL(void) { cpu_generic_LD_r8_u8(&m_reg.B, m_reg.L); }
static void cpu_opcode_LD_rB_iHL(void) {
    cpu_generic_LD_r8_u8(&m_reg.B, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_LD_rB_rA(void) { cpu_generic_LD_r8_u8(&m_reg.B, m_reg.A); }

// Load C from Register (or indirect HL)
static void cpu_opcode_LD_rC_rB(void) { cpu_generic_LD_r8_u8(&m_reg.C, m_reg.B); }
static void cpu_opcode_LD_rC_rC(void) { cpu_generic_LD_r8_u8(&m_reg.C, m_reg.C); }
static void cpu_opcode_LD_rC_rD(void) { cpu_generic_LD_r8_u8(&m_reg.C, m_reg.D); }
static void cpu_opcode_LD_rC_rE(void) { cpu_generic_LD_r8_u8(&m_reg.C, m_reg.E); }
static void cpu_opcode_LD_rC_rH(void) { cpu_generic_LD_r8_u8(&m_reg.C, m_reg.H); }
static void cpu_opcode_LD_rC_rL(void) { cpu_generic_LD_r8_u8(&m_reg.C, m_reg.L); }
static void cpu_opcode_LD_rC_iHL(void) {
    cpu_generic_LD_r8_u8(&m_reg.C, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_LD_rC_rA(void) { cpu_generic_LD_r8_u8(&m_reg.C, m_reg.A); }

// Load D from Register (or indirect HL)
static void cpu_opcode_LD_rD_rB(void) { cpu_generic_LD_r8_u8(&m_reg.D, m_reg.B); }
static void cpu_opcode_LD_rD_rC(void) { cpu_generic_LD_r8_u8(&m_reg.D, m_reg.C); }
static void cpu_opcode_LD_rD_rD(void) { cpu_generic_LD_r8_u8(&m_reg.D, m_reg.D); }
static void cpu_opcode_LD_rD_rE(void) { cpu_generic_LD_r8_u8(&m_reg.D, m_reg.E); }
static void cpu_opcode_LD_rD_rH(void) { cpu_generic_LD_r8_u8(&m_reg.D, m_reg.H); }
static void cpu_opcode_LD_rD_rL(void) { cpu_generic_LD_r8_u8(&m_reg.D, m_reg.L); }
static void cpu_opcode_LD_rD_iHL(void) {
    cpu_generic_LD_r8_u8(&m_reg.D, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_LD_rD_rA(void) { cpu_generic_LD_r8_u8(&m_reg.D, m_reg.A); }

// Load E from Register (or indirect HL)
static void cpu_opcode_LD_rE_rB(void) { cpu_generic_LD_r8_u8(&m_reg.E, m_reg.B); }
static void cpu_opcode_LD_rE_rC(void) { cpu_generic_LD_r8_u8(&m_reg.E, m_reg.C); }
static void cpu_opcode_LD_rE_rD(void) { cpu_generic_LD_r8_u8(&m_reg.E, m_reg.D); }
static void cpu_opcode_LD_rE_rE(void) { cpu_generic_LD_r8_u8(&m_reg.E, m_reg.E); }
static void cpu_opcode_LD_rE_rH(void) { cpu_generic_LD_r8_u8(&m_reg.E, m_reg.H); }
static void cpu_opcode_LD_rE_rL(void) { cpu_generic_LD_r8_u8(&m_reg.E, m_reg.L); }
static void cpu_opcode_LD_rE_iHL(void) {
    cpu_generic_LD_r8_u8(&m_reg.E, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_LD_rE_rA(void) { cpu_generic_LD_r8_u8(&m_reg.E, m_reg.A); }

// Load H from Register (or indirect HL)
static void cpu_opcode_LD_rH_rB(void) { cpu_generic_LD_r8_u8(&m_reg.H, m_reg.B); }
static void cpu_opcode_LD_rH_rC(void) { cpu_generic_LD_r8_u8(&m_reg.H, m_reg.C); }
static void cpu_opcode_LD_rH_rD(void) { cpu_generic_LD_r8_u8(&m_reg.H, m_reg.D); }
static void cpu_opcode_LD_rH_rE(void) { cpu_generic_LD_r8_u8(&m_reg.H, m_reg.E); }
static void cpu_opcode_LD_rH_rH(void) { cpu_generic_LD_r8_u8(&m_reg.H, m_reg.H); }
static void cpu_opcode_LD_rH_rL(void) { cpu_generic_LD_r8_u8(&m_reg.H, m_reg.L); }
static void cpu_opcode_LD_rH_iHL(void) {
    cpu_generic_LD_r8_u8(&m_reg.H, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_LD_rH_rA(void) { cpu_generic_LD_r8_u8(&m_reg.H, m_reg.A); }

// Load L from Register (or indirect HL)
static void cpu_opcode_LD_rL_rB(void) { cpu_generic_LD_r8_u8(&m_reg.L, m_reg.B); }
static void cpu_opcode_LD_rL_rC(void) { cpu_generic_LD_r8_u8(&m_reg.L, m_reg.C); }
static void cpu_opcode_LD_rL_rD(void) { cpu_generic_LD_r8_u8(&m_reg.L, m_reg.D); }
static void cpu_opcode_LD_rL_rE(void) { cpu_generic_LD_r8_u8(&m_reg.L, m_reg.E); }
static void cpu_opcode_LD_rL_rH(void) { cpu_generic_LD_r8_u8(&m_reg.L, m_reg.H); }
static void cpu_opcode_LD_rL_rL(void) { cpu_generic_LD_r8_u8(&m_reg.L, m_reg.L); }
static void cpu_opcode_LD_rL_iHL(void) {
    cpu_generic_LD_r8_u8(&m_reg.L, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_LD_rL_rA(void) { cpu_generic_LD_r8_u8(&m_reg.L, m_reg.A); }

// Load indirect HL from Register
static void cpu_opcode_LD_iHL_rB(void) { cpu_generic_LD_ir16_u8(m_reg.HL, m_reg.B); }
static void cpu_opcode_LD_iHL_rC(void) { cpu_generic_LD_ir16_u8(m_reg.HL, m_reg.C); }
static void cpu_opcode_LD_iHL_rD(void) { cpu_generic_LD_ir16_u8(m_reg.HL, m_reg.D); }
static void cpu_opcode_LD_iHL_rE(void) { cpu_generic_LD_ir16_u8(m_reg.HL, m_reg.E); }
static void cpu_opcode_LD_iHL_rH(void) { cpu_generic_LD_ir16_u8(m_reg.HL, m_reg.H); }
static void cpu_opcode_LD_iHL_rL(void) { cpu_generic_LD_ir16_u8(m_reg.HL, m_reg.L); }
// No LD (HL), (HL) instruction
static void cpu_opcode_LD_iHL_rA(void) { cpu_generic_LD_ir16_u8(m_reg.HL, m_reg.A); }

// Load A from Register (or indirect HL)
static void cpu_opcode_LD_rA_rB(void) { cpu_generic_LD_r8_u8(&m_reg.A, m_reg.B); }
static void cpu_opcode_LD_rA_rC(void) { cpu_generic_LD_r8_u8(&m_reg.A, m_reg.C); }
static void cpu_opcode_LD_rA_rD(void) { cpu_generic_LD_r8_u8(&m_reg.A, m_reg.D); }
static void cpu_opcode_LD_rA_rE(void) { cpu_generic_LD_r8_u8(&m_reg.A, m_reg.E); }
static void cpu_opcode_LD_rA_rH(void) { cpu_generic_LD_r8_u8(&m_reg.A, m_reg.H); }
static void cpu_opcode_LD_rA_rL(void) { cpu_generic_LD_r8_u8(&m_reg.A, m_reg.L); }
static void cpu_opcode_LD_rA_iHL(void) {
    cpu_generic_LD_r8_u8(&m_reg.A, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_LD_rA_rA(void) { cpu_generic_LD_r8_u8(&m_reg.A, m_reg.A); }

// Load High
// Used to quickly access IO registers
// Note: These can be tricky to read but are always in reference to A
static void cpu_opcode_LDH_u8_rA(void) { cpu_generic_LDH_u8_r8(cpu_read_u8(), m_reg.A); }
static void cpu_opcode_LDH_rA_u8(void) { cpu_generic_LDH_r8_u8(&m_reg.A, cpu_read_u8()); }
static void cpu_opcode_LDH_rC_rA(void) { cpu_generic_LDH_u8_r8(m_reg.C, m_reg.A); }
static void cpu_opcode_LDH_rA_rC(void) { cpu_generic_LDH_r8_u8(&m_reg.A, m_reg.C); }

// Load indirect immediate
static void cpu_opcode_LD_i16_rA(void) { cpu_generic_LD_ir16_u8(cpu_read_u16(), m_reg.A); }
static void cpu_opcode_LD_rA_i16(void) { cpu_generic_LD_r8_ir16(&m_reg.A, cpu_read_u16()); }

// ----- 8-bit Load/Store/Move -----

// ----- 16-bit Load/Store/Mode ----

// Load immediate
static void cpu_opcode_LD_rBC_u16(void) { cpu_generic_LD_r16_u16(&m_reg.BC, cpu_read_u16()); }
static void cpu_opcode_LD_rDE_u16(void) { cpu_generic_LD_r16_u16(&m_reg.DE, cpu_read_u16()); }
static void cpu_opcode_LD_rHL_u16(void) { cpu_generic_LD_r16_u16(&m_reg.HL, cpu_read_u16()); }
static void cpu_opcode_LD_rSP_u16(void) { cpu_generic_LD_r16_u16(&m_reg.SP, cpu_read_u16()); }

// Special SP
static void cpu_opcode_LD_i16_rSP(void);
static void cpu_opcode_LD_rSP_rHL(void);

// POP/PUSH
static void cpu_opcode_POP_rBC(void) { cpu_generic_POP_r16(&m_reg.BC); }
static void cpu_opcode_POP_rDE(void) { cpu_generic_POP_r16(&m_reg.DE); }
static void cpu_opcode_POP_rHL(void) { cpu_generic_POP_r16(&m_reg.HL); }
static void cpu_opcode_POP_rAF(void) { cpu_generic_POP_r16(&m_reg.AF); }
static void cpu_opcode_PUSH_rBC(void) { cpu_generic_PUSH_r16(m_reg.BC); }
static void cpu_opcode_PUSH_rDE(void) { cpu_generic_PUSH_r16(m_reg.DE); }
static void cpu_opcode_PUSH_rHL(void) { cpu_generic_PUSH_r16(m_reg.HL); }
static void cpu_opcode_PUSH_rAF(void) { cpu_generic_PUSH_r16(m_reg.AF); }

// ----- 16-bit Load/Store/Move -----

// ----- 8-bit ALU -----

// Increment Register (or indirect HL)
static void cpu_opcode_INC_rB(void) { cpu_generic_INC_r8(&m_reg.B); }
static void cpu_opcode_INC_rC(void) { cpu_generic_INC_r8(&m_reg.C); }
static void cpu_opcode_INC_rD(void) { cpu_generic_INC_r8(&m_reg.D); }
static void cpu_opcode_INC_rE(void) { cpu_generic_INC_r8(&m_reg.E); }
static void cpu_opcode_INC_rH(void) { cpu_generic_INC_r8(&m_reg.H); }
static void cpu_opcode_INC_rL(void) { cpu_generic_INC_r8(&m_reg.L); }
static void cpu_opcode_INC_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_INC_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_INC_rA(void) { cpu_generic_INC_r8(&m_reg.A); }

// Decrement Register (or indirect HL)
static void cpu_opcode_DEC_rB(void) { cpu_generic_DEC_r8(&m_reg.B); }
static void cpu_opcode_DEC_rC(void) { cpu_generic_DEC_r8(&m_reg.C); }
static void cpu_opcode_DEC_rD(void) { cpu_generic_DEC_r8(&m_reg.D); }
static void cpu_opcode_DEC_rE(void) { cpu_generic_DEC_r8(&m_reg.E); }
static void cpu_opcode_DEC_rH(void) { cpu_generic_DEC_r8(&m_reg.H); }
static void cpu_opcode_DEC_rL(void) { cpu_generic_DEC_r8(&m_reg.L); }
static void cpu_opcode_DEC_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_DEC_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_DEC_rA(void) { cpu_generic_DEC_r8(&m_reg.A); }

// Add Register (or indirect HL) to A
static void cpu_opcode_ADD_rA_rB(void) { cpu_generic_ADD_r8_u8(&m_reg.A, m_reg.B); }
static void cpu_opcode_ADD_rA_rC(void) { cpu_generic_ADD_r8_u8(&m_reg.A, m_reg.C); }
static void cpu_opcode_ADD_rA_rD(void) { cpu_generic_ADD_r8_u8(&m_reg.A, m_reg.D); }
static void cpu_opcode_ADD_rA_rE(void) { cpu_generic_ADD_r8_u8(&m_reg.A, m_reg.E); }
static void cpu_opcode_ADD_rA_rH(void) { cpu_generic_ADD_r8_u8(&m_reg.A, m_reg.H); }
static void cpu_opcode_ADD_rA_rL(void) { cpu_generic_ADD_r8_u8(&m_reg.A, m_reg.L); }
static void cpu_opcode_ADD_rA_iHL(void) {
    cpu_generic_ADD_r8_u8(&m_reg.A, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_ADD_rA_rA(void) { cpu_generic_ADD_r8_u8(&m_reg.A, m_reg.A); }

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
static void cpu_opcode_SUB_rA_rB(void) { cpu_generic_SUB_r8_u8(&m_reg.A, m_reg.B); }
static void cpu_opcode_SUB_rA_rC(void) { cpu_generic_SUB_r8_u8(&m_reg.A, m_reg.C); }
static void cpu_opcode_SUB_rA_rD(void) { cpu_generic_SUB_r8_u8(&m_reg.A, m_reg.D); }
static void cpu_opcode_SUB_rA_rE(void) { cpu_generic_SUB_r8_u8(&m_reg.A, m_reg.E); }
static void cpu_opcode_SUB_rA_rH(void) { cpu_generic_SUB_r8_u8(&m_reg.A, m_reg.H); }
static void cpu_opcode_SUB_rA_rL(void) { cpu_generic_SUB_r8_u8(&m_reg.A, m_reg.L); }
static void cpu_opcode_SUB_rA_iHL(void) {
    cpu_generic_SUB_r8_u8(&m_reg.A, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_SUB_rA_rA(void) { cpu_generic_SUB_r8_u8(&m_reg.A, m_reg.A); }

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
static void cpu_opcode_XOR_rA_rB(void) { cpu_generic_XOR_r8_u8(&m_reg.A, m_reg.B); }
static void cpu_opcode_XOR_rA_rC(void) { cpu_generic_XOR_r8_u8(&m_reg.A, m_reg.C); }
static void cpu_opcode_XOR_rA_rD(void) { cpu_generic_XOR_r8_u8(&m_reg.A, m_reg.D); }
static void cpu_opcode_XOR_rA_rE(void) { cpu_generic_XOR_r8_u8(&m_reg.A, m_reg.E); }
static void cpu_opcode_XOR_rA_rH(void) { cpu_generic_XOR_r8_u8(&m_reg.A, m_reg.H); }
static void cpu_opcode_XOR_rA_rL(void) { cpu_generic_XOR_r8_u8(&m_reg.A, m_reg.L); }
static void cpu_opcode_XOR_rA_iHL(void) {
    cpu_generic_XOR_r8_u8(&m_reg.A, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_XOR_rA_rA(void) { cpu_generic_XOR_r8_u8(&m_reg.A, m_reg.A); }

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
static void cpu_opcode_CP_rA_rB(void) { cpu_generic_CP_r8_u8(&m_reg.A, m_reg.B); }
static void cpu_opcode_CP_rA_rC(void) { cpu_generic_CP_r8_u8(&m_reg.A, m_reg.C); }
static void cpu_opcode_CP_rA_rD(void) { cpu_generic_CP_r8_u8(&m_reg.A, m_reg.D); }
static void cpu_opcode_CP_rA_rE(void) { cpu_generic_CP_r8_u8(&m_reg.A, m_reg.E); }
static void cpu_opcode_CP_rA_rH(void) { cpu_generic_CP_r8_u8(&m_reg.A, m_reg.H); }
static void cpu_opcode_CP_rA_rL(void) { cpu_generic_CP_r8_u8(&m_reg.A, m_reg.L); }
static void cpu_opcode_CP_rA_iHL(void) {
    cpu_generic_CP_r8_u8(&m_reg.A, mmu_read(BUS_CPU, m_reg.HL));
}
static void cpu_opcode_CP_rA_rA(void) { cpu_generic_CP_r8_u8(&m_reg.A, m_reg.A); }

// Immediate ALU operation with A
static void cpu_opcode_ADD_rA_u8(void) { cpu_generic_ADD_r8_u8(&m_reg.A, cpu_read_u8()); }
static void cpu_opcode_ADC_rA_u8(void);
static void cpu_opcode_SUB_rA_u8(void) { cpu_generic_SUB_r8_u8(&m_reg.A, cpu_read_u8()); }
static void cpu_opcode_SBC_rA_u8(void);
static void cpu_opcode_AND_rA_u8(void);
static void cpu_opcode_XOR_rA_u8(void) { cpu_generic_XOR_r8_u8(&m_reg.A, cpu_read_u8()); }
static void cpu_opcode_OR_rA_u8(void);
static void cpu_opcode_CP_rA_u8(void) { cpu_generic_CP_r8_u8(&m_reg.A, cpu_read_u8()); }

// Misc
static void cpu_opcode_DAA(void);
static void cpu_opcode_SCF(void);
static void cpu_opcode_CPL(void);
static void cpu_opcode_CCF(void);

// ----- 8-bit ALU -----

// ----- 16-bit ALU -----

// Increment Register
static inline void cpu_opcode_INC_rBC(void) { cpu_generic_INC_r16(&m_reg.BC); }
static inline void cpu_opcode_INC_rDE(void) { cpu_generic_INC_r16(&m_reg.DE); }
static inline void cpu_opcode_INC_rHL(void) { cpu_generic_INC_r16(&m_reg.HL); }
static inline void cpu_opcode_INC_rSP(void) { cpu_generic_INC_r16(&m_reg.SP); }

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

// These are specical varients of the rotate / shift functions. Flags are handled differently than
// the generic case
static void cpu_opcode_RLCA(void) {
    struct alu_result result = alu_rlc_8b(m_reg.A);
    m_reg.A                  = result.result.byte;
    registers_set_flags(&m_reg, false, false, false, result.flags.c);
}
static void cpu_opcode_RRCA(void) {
    struct alu_result result = alu_rrc_8b(m_reg.A);
    m_reg.A                  = result.result.byte;
    registers_set_flags(&m_reg, false, false, false, result.flags.c);
}
static void cpu_opcode_RLA(void) {
    struct alu_result result = alu_rl_8b(m_reg.A, registers_get_flag_c(&m_reg));
    m_reg.A                  = result.result.byte;
    registers_set_flags(&m_reg, false, false, false, result.flags.c);
}
static void cpu_opcode_RRA(void) {
    struct alu_result result = alu_rr_8b(m_reg.A, registers_get_flag_c(&m_reg));
    m_reg.A                  = result.result.byte;
    registers_set_flags(&m_reg, false, false, false, result.flags.c);
}

// ----- 8-bit Rotate/Shift -----

// ========== Standard Opcodes ==========

// ========== CB Prefix Opcodes ==========

// ----- 8-bit Rotate/Shift -----

// Rotate Left Circular
static void cpu_opcode_RLC_rB(void) { cpu_generic_RLC_r8(&m_reg.B); }
static void cpu_opcode_RLC_rC(void) { cpu_generic_RLC_r8(&m_reg.C); }
static void cpu_opcode_RLC_rD(void) { cpu_generic_RLC_r8(&m_reg.D); }
static void cpu_opcode_RLC_rE(void) { cpu_generic_RLC_r8(&m_reg.E); }
static void cpu_opcode_RLC_rH(void) { cpu_generic_RLC_r8(&m_reg.H); }
static void cpu_opcode_RLC_rL(void) { cpu_generic_RLC_r8(&m_reg.L); }
static void cpu_opcode_RLC_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_RLC_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_RLC_rA(void) { cpu_generic_RLC_r8(&m_reg.A); }

// Rotate Right Circular
static void cpu_opcode_RRC_rB(void) { cpu_generic_RRC_r8(&m_reg.B); }
static void cpu_opcode_RRC_rC(void) { cpu_generic_RRC_r8(&m_reg.C); }
static void cpu_opcode_RRC_rD(void) { cpu_generic_RRC_r8(&m_reg.D); }
static void cpu_opcode_RRC_rE(void) { cpu_generic_RRC_r8(&m_reg.E); }
static void cpu_opcode_RRC_rH(void) { cpu_generic_RRC_r8(&m_reg.H); }
static void cpu_opcode_RRC_rL(void) { cpu_generic_RRC_r8(&m_reg.L); }
static void cpu_opcode_RRC_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_RRC_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_RRC_rA(void) { cpu_generic_RRC_r8(&m_reg.A); }

// Rotate Left
static void cpu_opcode_RL_rB(void) { cpu_generic_RL_r8(&m_reg.B); }
static void cpu_opcode_RL_rC(void) { cpu_generic_RL_r8(&m_reg.C); }
static void cpu_opcode_RL_rD(void) { cpu_generic_RL_r8(&m_reg.D); }
static void cpu_opcode_RL_rE(void) { cpu_generic_RL_r8(&m_reg.E); }
static void cpu_opcode_RL_rH(void) { cpu_generic_RL_r8(&m_reg.H); }
static void cpu_opcode_RL_rL(void) { cpu_generic_RL_r8(&m_reg.L); }
static void cpu_opcode_RL_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_RL_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_RL_rA(void) { cpu_generic_RL_r8(&m_reg.A); }

// Rotate Right
static void cpu_opcode_RR_rB(void) { cpu_generic_RR_r8(&m_reg.B); }
static void cpu_opcode_RR_rC(void) { cpu_generic_RR_r8(&m_reg.C); }
static void cpu_opcode_RR_rD(void) { cpu_generic_RR_r8(&m_reg.D); }
static void cpu_opcode_RR_rE(void) { cpu_generic_RR_r8(&m_reg.E); }
static void cpu_opcode_RR_rH(void) { cpu_generic_RR_r8(&m_reg.H); }
static void cpu_opcode_RR_rL(void) { cpu_generic_RR_r8(&m_reg.L); }
static void cpu_opcode_RR_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_RR_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_RR_rA(void) { cpu_generic_RR_r8(&m_reg.A); }

// Shift Left Arithmetic
static void cpu_opcode_SLA_rB(void) { cpu_generic_SLA_r8(&m_reg.B); }
static void cpu_opcode_SLA_rC(void) { cpu_generic_SLA_r8(&m_reg.C); }
static void cpu_opcode_SLA_rD(void) { cpu_generic_SLA_r8(&m_reg.D); }
static void cpu_opcode_SLA_rE(void) { cpu_generic_SLA_r8(&m_reg.E); }
static void cpu_opcode_SLA_rH(void) { cpu_generic_SLA_r8(&m_reg.H); }
static void cpu_opcode_SLA_rL(void) { cpu_generic_SLA_r8(&m_reg.L); }
static void cpu_opcode_SLA_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_SLA_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_SLA_rA(void) { cpu_generic_SLA_r8(&m_reg.A); }

// Shift Right Arithmetic
static void cpu_opcode_SRA_rB(void) { cpu_generic_SRA_r8(&m_reg.B); }
static void cpu_opcode_SRA_rC(void) { cpu_generic_SRA_r8(&m_reg.C); }
static void cpu_opcode_SRA_rD(void) { cpu_generic_SRA_r8(&m_reg.D); }
static void cpu_opcode_SRA_rE(void) { cpu_generic_SRA_r8(&m_reg.E); }
static void cpu_opcode_SRA_rH(void) { cpu_generic_SRA_r8(&m_reg.H); }
static void cpu_opcode_SRA_rL(void) { cpu_generic_SRA_r8(&m_reg.L); }
static void cpu_opcode_SRA_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_SRA_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_SRA_rA(void) { cpu_generic_SRA_r8(&m_reg.A); }

// Swap the hi and lo nibble of a register (or indirect HL)
static void cpu_opcode_SWAP_rB(void) { cpu_generic_SWAP_r8(&m_reg.B); }
static void cpu_opcode_SWAP_rC(void) { cpu_generic_SWAP_r8(&m_reg.C); }
static void cpu_opcode_SWAP_rD(void) { cpu_generic_SWAP_r8(&m_reg.D); }
static void cpu_opcode_SWAP_rE(void) { cpu_generic_SWAP_r8(&m_reg.E); }
static void cpu_opcode_SWAP_rH(void) { cpu_generic_SWAP_r8(&m_reg.H); }
static void cpu_opcode_SWAP_rL(void) { cpu_generic_SWAP_r8(&m_reg.L); }
static void cpu_opcode_SWAP_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_SWAP_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_SWAP_rA(void) { cpu_generic_SWAP_r8(&m_reg.A); }

// Shift Right Logical
static void cpu_opcode_SRL_rB(void) { cpu_generic_SRL_r8(&m_reg.B); }
static void cpu_opcode_SRL_rC(void) { cpu_generic_SRL_r8(&m_reg.C); }
static void cpu_opcode_SRL_rD(void) { cpu_generic_SRL_r8(&m_reg.D); }
static void cpu_opcode_SRL_rE(void) { cpu_generic_SRL_r8(&m_reg.E); }
static void cpu_opcode_SRL_rH(void) { cpu_generic_SRL_r8(&m_reg.H); }
static void cpu_opcode_SRL_rL(void) { cpu_generic_SRL_r8(&m_reg.L); }
static void cpu_opcode_SRL_iHL(void) {
    byte value = mmu_read(BUS_CPU, m_reg.HL);
    cpu_generic_SRL_r8(&value);
    mmu_write(BUS_CPU, m_reg.HL, value);
}
static void cpu_opcode_SRL_rA(void) { cpu_generic_SRL_r8(&m_reg.A); }

// ----- 8-bit Rotate/Shift -----

// ----- 8-bit Bit Operations -----

// Get bit
static void cpu_opcode_BIT_0_rB(void) { cpu_generic_BIT(0, m_reg.B); }
static void cpu_opcode_BIT_0_rC(void) { cpu_generic_BIT(0, m_reg.C); }
static void cpu_opcode_BIT_0_rD(void) { cpu_generic_BIT(0, m_reg.D); }
static void cpu_opcode_BIT_0_rE(void) { cpu_generic_BIT(0, m_reg.E); }
static void cpu_opcode_BIT_0_rH(void) { cpu_generic_BIT(0, m_reg.H); }
static void cpu_opcode_BIT_0_rL(void) { cpu_generic_BIT(0, m_reg.L); }
static void cpu_opcode_BIT_0_iHL(void) { cpu_generic_BIT(0, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_0_rA(void) { cpu_generic_BIT(0, m_reg.A); }
static void cpu_opcode_BIT_1_rB(void) { cpu_generic_BIT(1, m_reg.B); }
static void cpu_opcode_BIT_1_rC(void) { cpu_generic_BIT(1, m_reg.C); }
static void cpu_opcode_BIT_1_rD(void) { cpu_generic_BIT(1, m_reg.D); }
static void cpu_opcode_BIT_1_rE(void) { cpu_generic_BIT(1, m_reg.E); }
static void cpu_opcode_BIT_1_rH(void) { cpu_generic_BIT(1, m_reg.H); }
static void cpu_opcode_BIT_1_rL(void) { cpu_generic_BIT(1, m_reg.L); }
static void cpu_opcode_BIT_1_iHL(void) { cpu_generic_BIT(1, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_1_rA(void) { cpu_generic_BIT(1, m_reg.A); }
static void cpu_opcode_BIT_2_rB(void) { cpu_generic_BIT(2, m_reg.B); }
static void cpu_opcode_BIT_2_rC(void) { cpu_generic_BIT(2, m_reg.C); }
static void cpu_opcode_BIT_2_rD(void) { cpu_generic_BIT(2, m_reg.D); }
static void cpu_opcode_BIT_2_rE(void) { cpu_generic_BIT(2, m_reg.E); }
static void cpu_opcode_BIT_2_rH(void) { cpu_generic_BIT(2, m_reg.H); }
static void cpu_opcode_BIT_2_rL(void) { cpu_generic_BIT(2, m_reg.L); }
static void cpu_opcode_BIT_2_iHL(void) { cpu_generic_BIT(2, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_2_rA(void) { cpu_generic_BIT(2, m_reg.A); }
static void cpu_opcode_BIT_3_rB(void) { cpu_generic_BIT(3, m_reg.B); }
static void cpu_opcode_BIT_3_rC(void) { cpu_generic_BIT(3, m_reg.C); }
static void cpu_opcode_BIT_3_rD(void) { cpu_generic_BIT(3, m_reg.D); }
static void cpu_opcode_BIT_3_rE(void) { cpu_generic_BIT(3, m_reg.E); }
static void cpu_opcode_BIT_3_rH(void) { cpu_generic_BIT(3, m_reg.H); }
static void cpu_opcode_BIT_3_rL(void) { cpu_generic_BIT(3, m_reg.L); }
static void cpu_opcode_BIT_3_iHL(void) { cpu_generic_BIT(3, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_3_rA(void) { cpu_generic_BIT(3, m_reg.A); }
static void cpu_opcode_BIT_4_rB(void) { cpu_generic_BIT(4, m_reg.B); }
static void cpu_opcode_BIT_4_rC(void) { cpu_generic_BIT(4, m_reg.C); }
static void cpu_opcode_BIT_4_rD(void) { cpu_generic_BIT(4, m_reg.D); }
static void cpu_opcode_BIT_4_rE(void) { cpu_generic_BIT(4, m_reg.E); }
static void cpu_opcode_BIT_4_rH(void) { cpu_generic_BIT(4, m_reg.H); }
static void cpu_opcode_BIT_4_rL(void) { cpu_generic_BIT(4, m_reg.L); }
static void cpu_opcode_BIT_4_iHL(void) { cpu_generic_BIT(4, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_4_rA(void) { cpu_generic_BIT(4, m_reg.A); }
static void cpu_opcode_BIT_5_rB(void) { cpu_generic_BIT(5, m_reg.B); }
static void cpu_opcode_BIT_5_rC(void) { cpu_generic_BIT(5, m_reg.C); }
static void cpu_opcode_BIT_5_rD(void) { cpu_generic_BIT(5, m_reg.D); }
static void cpu_opcode_BIT_5_rE(void) { cpu_generic_BIT(5, m_reg.E); }
static void cpu_opcode_BIT_5_rH(void) { cpu_generic_BIT(5, m_reg.H); }
static void cpu_opcode_BIT_5_rL(void) { cpu_generic_BIT(5, m_reg.L); }
static void cpu_opcode_BIT_5_iHL(void) { cpu_generic_BIT(5, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_5_rA(void) { cpu_generic_BIT(5, m_reg.A); }
static void cpu_opcode_BIT_6_rB(void) { cpu_generic_BIT(6, m_reg.B); }
static void cpu_opcode_BIT_6_rC(void) { cpu_generic_BIT(6, m_reg.C); }
static void cpu_opcode_BIT_6_rD(void) { cpu_generic_BIT(6, m_reg.D); }
static void cpu_opcode_BIT_6_rE(void) { cpu_generic_BIT(6, m_reg.E); }
static void cpu_opcode_BIT_6_rH(void) { cpu_generic_BIT(6, m_reg.H); }
static void cpu_opcode_BIT_6_rL(void) { cpu_generic_BIT(6, m_reg.L); }
static void cpu_opcode_BIT_6_iHL(void) { cpu_generic_BIT(6, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_6_rA(void) { cpu_generic_BIT(6, m_reg.A); }
static void cpu_opcode_BIT_7_rB(void) { cpu_generic_BIT(7, m_reg.B); }
static void cpu_opcode_BIT_7_rC(void) { cpu_generic_BIT(7, m_reg.C); }
static void cpu_opcode_BIT_7_rD(void) { cpu_generic_BIT(7, m_reg.D); }
static void cpu_opcode_BIT_7_rE(void) { cpu_generic_BIT(7, m_reg.E); }
static void cpu_opcode_BIT_7_rH(void) { cpu_generic_BIT(7, m_reg.H); }
static void cpu_opcode_BIT_7_rL(void) { cpu_generic_BIT(7, m_reg.L); }
static void cpu_opcode_BIT_7_iHL(void) { cpu_generic_BIT(7, mmu_read(BUS_CPU, m_reg.HL)); }
static void cpu_opcode_BIT_7_rA(void) { cpu_generic_BIT(7, m_reg.A); }

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
