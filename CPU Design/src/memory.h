#pragma once
// =============================================================================
//  SPARTAN-16  –  Physical RAM
// =============================================================================
//  Memory is a flat 64 KB byte array.  Address 0x0000…0xEFFF is RAM.
//  Addresses 0xF000…0xFFFF are intercepted by the Bus for MMIO; reads/writes
//  that reach Memory directly are treated as plain RAM (used as backing store
//  for any MMIO registers that need it).
//
//  All multi-byte accesses are little-endian (low byte at the lower address).
// =============================================================================
#include "common.h"
#include <array>

class Memory {
public:
    Memory();

    // ── Byte access ───────────────────────────────────────────────────────────
    u8   read8 (u16 addr) const;
    void write8(u16 addr, u8 val);

    // ── 16-bit word access (little-endian) ────────────────────────────────────
    u16  read16 (u16 addr) const;
    void write16(u16 addr, u16 val);

    // ── 32-bit word access (little-endian, used for instruction fetch) ────────
    u32  read32(u16 addr) const;

    // ── Bulk load (assembler / emulator boot) ─────────────────────────────────
    //  Copies `len` bytes from `src` into memory starting at `dest_addr`.
    void load(u16 dest_addr, const u8* src, u16 len);

    // ── Raw access for hex-dump ────────────────────────────────────────────────
    const std::array<u8, MEM_SIZE>& raw() const { return data_; }

    void reset();   // zero all RAM

private:
    std::array<u8, MEM_SIZE> data_;
};
