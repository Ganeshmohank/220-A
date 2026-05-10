#pragma once
// =============================================================================
//  SPARTAN-16  –  CPU State (register file + flags + PC + cycle counter)
// =============================================================================
#include "isa.h"
#include <string>

struct CPUState {
    // ── General-purpose & special registers ───────────────────────────────────
    //  Index 0-7  : R0 … R7  (general purpose)
    //  Index 8    : SP        (stack pointer)
    //  Index 9    : LR        (link register – holds return address after CALL)
    u16 regs[NUM_REGS] = {};

    // ── Program counter ───────────────────────────────────────────────────────
    u16 pc = RESET_VECTOR;

    // ── Status flags register ─────────────────────────────────────────────────
    //  Bit 0 – Z (Zero)      : last ALU result was 0
    //  Bit 1 – N (Negative)  : last ALU result's MSB was 1
    //  Bit 2 – C (Carry)     : unsigned overflow or borrow
    //  Bit 3 – V (Overflow)  : signed overflow
    u16 flags = 0;

    // ── Cycle counter (incremented once per executed instruction) ─────────────
    u64 cycles = 0;

    // ── Halt flag (set by HLT instruction) ───────────────────────────────────
    bool halted = false;

    // ── Flag accessors ────────────────────────────────────────────────────────
    bool flag_z() const { return (flags & FLAG_Z) != 0; }
    bool flag_n() const { return (flags & FLAG_N) != 0; }
    bool flag_c() const { return (flags & FLAG_C) != 0; }
    bool flag_v() const { return (flags & FLAG_V) != 0; }

    // ── Safe register read (returns 0 for out-of-range index) ────────────────
    u16 reg(u8 idx) const {
        return (idx < NUM_REGS) ? regs[idx] : 0u;
    }

    // ── Reset to power-on state ───────────────────────────────────────────────
    void reset();

    // ── Pretty-print register file (used by --dump) ───────────────────────────
    void dump() const;

    // ── One-line summary of flags for trace mode ──────────────────────────────
    std::string flags_str() const;
};
