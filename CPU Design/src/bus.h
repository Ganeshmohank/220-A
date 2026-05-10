#pragma once
// =============================================================================
//  SPARTAN-16  –  System Bus
// =============================================================================
//  The Bus sits between the Control Unit and all addressable resources.
//  It inspects every address and routes the access to either:
//    • Physical RAM  (0x0000 – 0xEFFF)
//    • MMIO region   (0xF000 – 0xFFFF)
//
//  MMIO is handled entirely inside the Bus:
//    0xF000  STDOUT_CHAR  – write: lower byte output as ASCII character
//    0xF001  STDIN        – read:  one character from stdin (blocking)
//    0xF002  STDOUT_NUM   – write: 16-bit value printed as decimal + newline
//    0xF004  TIMER_LO     – read:  cpu.cycles[15:0]
//    0xF006  TIMER_HI     – read:  cpu.cycles[31:16]
// =============================================================================
#include "memory.h"
#include "cpu.h"

class Bus {
public:
    // The Bus holds references; it does NOT own Memory or CPUState.
    Bus(Memory& mem, CPUState& cpu) : mem_(mem), cpu_(cpu) {}

    // ── 16-bit word access (used by LOAD / STORE and instruction fetch) ────────
    u16  readWord (u16 addr);
    void writeWord(u16 addr, u16 val);

    // ── Byte access (used by LOADB / STOREB) ──────────────────────────────────
    u8   readByte (u16 addr);
    void writeByte(u16 addr, u8 val);

    // ── 32-bit fetch (Control Unit fetches 4-byte instructions) ──────────────
    u32 fetch32(u16 addr);

private:
    Memory&   mem_;
    CPUState& cpu_;

    bool is_mmio(u16 addr) const { return addr >= MMIO_BASE; }

    u16  mmio_read (u16 addr);
    void mmio_write(u16 addr, u16 val);
};
