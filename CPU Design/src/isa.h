#pragma once
// =============================================================================
//  SPARTAN-16  –  Instruction Set Architecture
// =============================================================================
//
//  All instructions are exactly 32 bits wide (4 bytes), stored little-endian.
//
//  ┌─────────────────────────────────────────────────────┐
//  │ Bit layout                                           │
//  │                                                      │
//  │  31      24  23   20  19   16  15               0   │
//  │ ┌──────────┬────────┬────────┬──────────────────┐   │
//  │ │  opcode  │   rd   │  rs1   │     imm16         │   │ I-type
//  │ └──────────┴────────┴────────┴──────────────────┘   │
//  │                                                      │
//  │  31      24  23   20  19   16  15  12  11        0  │
//  │ ┌──────────┬────────┬────────┬───────┬───────────┐  │
//  │ │  opcode  │   rd   │  rs1   │  rs2  │   (zero)  │  │ R-type
//  │ └──────────┴────────┴────────┴───────┴───────────┘  │
//  └─────────────────────────────────────────────────────┘
//
//  For R-type, rs2 occupies bits [15:12] of the 32-bit word (upper nibble of
//  the imm16 field).  Bits [11:0] are ignored.
//
//  For STORE / STOREB / PUSH, the "rd" field carries the *source* register
//  (the value being written), since there is no register destination.
//
// =============================================================================
#include "common.h"

// ── Register file ─────────────────────────────────────────────────────────────
static constexpr u8  REG_SP   = 8;   // stack pointer  (alias for R8)
static constexpr u8  REG_LR   = 9;   // link register  (alias for R9, holds return address)
static constexpr u8  NUM_REGS = 10;  // R0 … R7, SP(R8), LR(R9)

// ── Status-flag bit positions inside the FLAGS register ───────────────────────
static constexpr u16 FLAG_Z = (1u << 0);  // Zero     – result == 0
static constexpr u16 FLAG_N = (1u << 1);  // Negative – result[15] == 1
static constexpr u16 FLAG_C = (1u << 2);  // Carry    – unsigned overflow / borrow
static constexpr u16 FLAG_V = (1u << 3);  // Overflow – signed overflow

// ── Opcode table ─────────────────────────────────────────────────────────────
enum Opcode : u8 {
    // Misc
    NOP    = 0x00,  // No operation
    HLT    = 0x01,  // Halt CPU

    // R-type ALU  (rd = rs1  OP  rs2)
    ADD    = 0x02,
    SUB    = 0x03,
    AND    = 0x04,
    OR     = 0x05,
    XOR    = 0x06,
    NOT    = 0x07,  // rd = ~rs1           (rs2 unused)
    SHL    = 0x08,  // rd = rs1 << rs2     (logical)
    SHR    = 0x09,  // rd = rs1 >> rs2     (logical)
    MUL    = 0x0A,
    DIV    = 0x0B,
    MOD    = 0x0C,
    CMP    = 0x0D,  // set flags(rs1 – rs2), discard result
    MOV    = 0x0E,  // rd = rs1

    // I-type ALU  (rd = rs1  OP  sign_ext(imm16))
    ADDI   = 0x10,
    SUBI   = 0x11,
    ANDI   = 0x12,
    ORI    = 0x13,
    XORI   = 0x14,
    LI     = 0x15,  // rd = imm16          (load immediate, rs1 unused)
    CMPI   = 0x16,  // set flags(rs1 – imm16), discard result

    // Memory access  (address = rs1 + sign_ext(imm16))
    LOAD   = 0x20,  // rd  = mem16[addr]
    STORE  = 0x21,  // mem16[addr] = rd    (rd field = source register)
    LOADB  = 0x22,  // rd  = zero_ext(mem8[addr])
    STOREB = 0x23,  // mem8[addr]  = rd[7:0]

    // Control flow
    JMP    = 0x30,  // PC = imm16          (absolute jump)
    JMPR   = 0x31,  // PC = rs1            (register-indirect jump)
    JZ     = 0x32,  // if  Z: PC = imm16
    JNZ    = 0x33,  // if !Z: PC = imm16
    JN     = 0x34,  // if  N: PC = imm16
    JNN    = 0x35,  // if !N: PC = imm16
    JC     = 0x36,  // if  C: PC = imm16
    JV     = 0x37,  // if  V: PC = imm16
    CALL   = 0x38,  // LR = PC+4 (already advanced); PC = imm16
    CALLR  = 0x39,  // LR = PC+4; PC = rs1
    RET    = 0x3A,  // PC = LR

    // Stack  (SP grows downward; push decrements, pop increments)
    PUSH   = 0x40,  // SP -= 2; mem16[SP] = rd   (rd = source)
    POP    = 0x41,  // rd = mem16[SP]; SP += 2
};

// ── Decoded instruction ───────────────────────────────────────────────────────
struct Instruction {
    Opcode opcode;
    u8     rd;   // destination (or source for STORE/STOREB/PUSH)
    u8     rs1;  // source register 1
    u16    imm;  // 16-bit immediate; for R-type, rs2 = imm[15:12]
};

// Extract rs2 from an R-type instruction
inline u8 rs2_of(const Instruction& i) {
    return static_cast<u8>((i.imm >> 12) & 0xFu);
}

// Encode a 32-bit instruction word from components
inline u32 encode_instruction(Opcode op, u8 rd, u8 rs1, u16 imm) {
    return (static_cast<u32>(op)        << 24)
         | (static_cast<u32>(rd  & 0xF) << 20)
         | (static_cast<u32>(rs1 & 0xF) << 16)
         | static_cast<u32>(imm);
}

// Convenience: build an R-type word (rs2 packed into bits [15:12])
inline u32 encode_rtype(Opcode op, u8 rd, u8 rs1, u8 rs2) {
    return encode_instruction(op, rd, rs1,
                              static_cast<u16>(rs2 & 0xFu) << 12);
}

// Decode a 32-bit word into an Instruction struct
inline Instruction decode_instruction(u32 word) {
    return {
        static_cast<Opcode>( word >> 24        ),
        static_cast<u8>(    (word >> 20) & 0xFu),
        static_cast<u8>(    (word >> 16) & 0xFu),
        static_cast<u16>(    word        & 0xFFFFu)
    };
}

// Human-readable mnemonic string (defined in isa.cpp)
const char* opcode_name(Opcode op);

// Human-readable register name (R0…R7, SP, LR)
const char* reg_name(u8 reg);

// Disassemble one instruction word into a printable string
std::string disassemble(u32 word);
