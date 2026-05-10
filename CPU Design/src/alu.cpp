// =============================================================================
//  SPARTAN-16  –  ALU implementation
// =============================================================================
#include "alu.h"

// ── Internal helper: flags for logical ops (C and V are always cleared) ────────
u16 ALU::logic_flags(u16 result) {
    u16 f = 0;
    if (result == 0)        f |= FLAG_Z;
    if (result & 0x8000u)   f |= FLAG_N;
    return f;
}

// ── ADD ──────────────────────────────────────────────────────────────────────
//  Uses a 32-bit intermediate to detect carry out of bit 15.
//  Signed overflow (V) occurs when two operands of the same sign produce
//  a result of the opposite sign.
ALUResult ALU::add(u16 a, u16 b) const {
    u32 r32 = static_cast<u32>(a) + static_cast<u32>(b);
    u16 r16 = static_cast<u16>(r32);

    u16 f = 0;
    if (r16 == 0)                              f |= FLAG_Z;
    if (r16 & 0x8000u)                         f |= FLAG_N;
    if (r32 >> 16)                             f |= FLAG_C;  // carry out
    // V: (~(a^b) & (a^r16)) tests whether sign changed unexpectedly
    if (((~(a ^ b) & (a ^ r16)) >> 15) & 1u)  f |= FLAG_V;

    return {r16, f};
}

// ── SUB ──────────────────────────────────────────────────────────────────────
//  a – b.  C flag signals unsigned borrow (set when a < b unsigned).
//  V flag signals signed overflow.
ALUResult ALU::sub(u16 a, u16 b) const {
    u32 r32 = static_cast<u32>(a) - static_cast<u32>(b);
    u16 r16 = static_cast<u16>(r32);

    u16 f = 0;
    if (r16 == 0)                             f |= FLAG_Z;
    if (r16 & 0x8000u)                        f |= FLAG_N;
    if (a < b)                                f |= FLAG_C;  // unsigned borrow
    // V: overflow if operands differ in sign and result differs from minuend
    if (((( a ^ b) & (a ^ r16)) >> 15) & 1u) f |= FLAG_V;

    return {r16, f};
}

// ── MUL ───────────────────────────────────────────────────────────────────────
//  Unsigned 16×16 → lower 16 bits.  C is set if the upper 16 bits are non-zero
//  (product didn't fit in 16 bits).
ALUResult ALU::mul(u16 a, u16 b) const {
    u32 r32 = static_cast<u32>(a) * static_cast<u32>(b);
    u16 r16 = static_cast<u16>(r32);
    u16 f   = logic_flags(r16);
    if (r32 >> 16) f |= FLAG_C;   // upper half non-zero → overflow
    return {r16, f};
}

// ── DIV ───────────────────────────────────────────────────────────────────────
//  Unsigned integer division.  Division by zero yields 0 (no trap).
ALUResult ALU::div_op(u16 a, u16 b) const {
    u16 r = (b == 0) ? 0u : static_cast<u16>(a / b);
    return {r, logic_flags(r)};
}

// ── MOD ───────────────────────────────────────────────────────────────────────
//  Unsigned remainder.  Modulo by zero yields 0.
ALUResult ALU::mod_op(u16 a, u16 b) const {
    u16 r = (b == 0) ? 0u : static_cast<u16>(a % b);
    return {r, logic_flags(r)};
}

// ── AND / OR / XOR / NOT ──────────────────────────────────────────────────────
ALUResult ALU::bitwise_and(u16 a, u16 b) const {
    u16 r = a & b;
    return {r, logic_flags(r)};
}

ALUResult ALU::bitwise_or(u16 a, u16 b) const {
    u16 r = a | b;
    return {r, logic_flags(r)};
}

ALUResult ALU::bitwise_xor(u16 a, u16 b) const {
    u16 r = a ^ b;
    return {r, logic_flags(r)};
}

ALUResult ALU::bitwise_not(u16 a) const {
    u16 r = ~a;
    return {r, logic_flags(r)};
}

// ── SHL (logical left shift) ──────────────────────────────────────────────────
//  Shift amount is taken mod 16 (bits [3:0] of `shift`).
//  C is set to the last bit shifted out (bit 15 of `a` before shift, if any).
ALUResult ALU::shl(u16 a, u16 shift) const {
    u16 amount = shift & 0xFu;

    if (amount == 0) return {a, logic_flags(a)};

    // The bit that will be shifted out is bit (16 – amount) of a
    u16 f   = 0;
    u32 r32 = static_cast<u32>(a) << amount;
    u16 r16 = static_cast<u16>(r32);

    if (r16 == 0)         f |= FLAG_Z;
    if (r16 & 0x8000u)    f |= FLAG_N;
    if ((r32 >> 16) & 1u) f |= FLAG_C;   // last bit shifted out

    return {r16, f};
}

// ── SHR (logical right shift) ─────────────────────────────────────────────────
ALUResult ALU::shr(u16 a, u16 shift) const {
    u16 amount = shift & 0xFu;

    if (amount == 0) return {a, logic_flags(a)};

    // C = last bit shifted out (bit amount-1 of original a)
    u16 last_out = (a >> (amount - 1)) & 1u;
    u16 r16      = a >> amount;
    u16 f        = logic_flags(r16);
    if (last_out) f |= FLAG_C;

    return {r16, f};
}
