// =============================================================================
//  SPARTAN-16  –  Physical RAM implementation
// =============================================================================
#include "memory.h"
#include <algorithm>
#include <stdexcept>

Memory::Memory() {
    data_.fill(0);
}

void Memory::reset() {
    data_.fill(0);
}

// ── Byte access ───────────────────────────────────────────────────────────────
u8 Memory::read8(u16 addr) const {
    return data_[addr];
}

void Memory::write8(u16 addr, u8 val) {
    data_[addr] = val;
}

// ── 16-bit little-endian word access ──────────────────────────────────────────
//  The low byte lives at addr, the high byte at addr+1.
//  Address wrap-around at the 64 KB boundary is handled naturally by u16.
u16 Memory::read16(u16 addr) const {
    return static_cast<u16>(data_[addr])
         | static_cast<u16>(static_cast<u16>(data_[static_cast<u16>(addr + 1)]) << 8);
}

void Memory::write16(u16 addr, u16 val) {
    data_[addr]                         = static_cast<u8>(val & 0xFF);
    data_[static_cast<u16>(addr + 1)]   = static_cast<u8>(val >> 8);
}

// ── 32-bit little-endian word access (instruction fetch) ──────────────────────
u32 Memory::read32(u16 addr) const {
    u32 lo = read16(addr);
    u32 hi = read16(static_cast<u16>(addr + 2));
    return lo | (hi << 16);
}

// ── Bulk load ─────────────────────────────────────────────────────────────────
void Memory::load(u16 dest_addr, const u8* src, u16 len) {
    for (u16 i = 0; i < len; ++i)
        data_[static_cast<u16>(dest_addr + i)] = src[i];
}
