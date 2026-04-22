#pragma once
// =============================================================================
//  SPARTAN-16  –  Arithmetic Logic Unit
// =============================================================================
//  The ALU is stateless: every method takes operand values and returns an
//  ALUResult containing the computed value AND the updated FLAGS register.
//  The caller (ControlUnit) decides which result fields to commit to the CPU.
// =============================================================================
#include "isa.h"

// Result bundle returned by every ALU operation
struct ALUResult {
    u16 value;  // computed result (16-bit)
    u16 flags;  // new FLAGS register  (FLAG_Z | FLAG_N | FLAG_C | FLAG_V bits)
};

class ALU {
public:
    // ── Arithmetic ────────────────────────────────────────────────────────────
    ALUResult add(u16 a, u16 b) const;
    ALUResult sub(u16 a, u16 b) const;
    ALUResult mul(u16 a, u16 b) const;
    ALUResult div_op(u16 a, u16 b) const;  // named div_op to avoid macro clash
    ALUResult mod_op(u16 a, u16 b) const;

    // ── Bitwise logic ─────────────────────────────────────────────────────────
    ALUResult bitwise_and(u16 a, u16 b) const;
    ALUResult bitwise_or (u16 a, u16 b) const;
    ALUResult bitwise_xor(u16 a, u16 b) const;
    ALUResult bitwise_not(u16 a)        const;

    // ── Shifts ────────────────────────────────────────────────────────────────
    ALUResult shl(u16 a, u16 shift) const;  // logical left
    ALUResult shr(u16 a, u16 shift) const;  // logical right

private:
    // Build a FLAGS word from a result value (sets Z and N; leaves C and V = 0)
    static u16 logic_flags(u16 result);
};
