// =============================================================================
//  SPARTAN-16  –  System Bus implementation
// =============================================================================
#include "bus.h"
#include <cstdio>

// ── Word read ─────────────────────────────────────────────────────────────────
u16 Bus::readWord(u16 addr) {
    if (is_mmio(addr)) return mmio_read(addr);
    return mem_.read16(addr);
}

// ── Word write ────────────────────────────────────────────────────────────────
void Bus::writeWord(u16 addr, u16 val) {
    if (is_mmio(addr)) { mmio_write(addr, val); return; }
    mem_.write16(addr, val);
}

// ── Byte read ─────────────────────────────────────────────────────────────────
u8 Bus::readByte(u16 addr) {
    if (is_mmio(addr)) return static_cast<u8>(mmio_read(addr) & 0xFF);
    return mem_.read8(addr);
}

// ── Byte write ────────────────────────────────────────────────────────────────
void Bus::writeByte(u16 addr, u8 val) {
    if (is_mmio(addr)) { mmio_write(addr, static_cast<u16>(val)); return; }
    mem_.write8(addr, val);
}

// ── 32-bit instruction fetch (always from RAM; PC cannot be in MMIO) ──────────
u32 Bus::fetch32(u16 addr) {
    return mem_.read32(addr);
}

// ── MMIO read ─────────────────────────────────────────────────────────────────
u16 Bus::mmio_read(u16 addr) {
    switch (addr) {
    case MMIO_STDIN:
        // Block and read one character from the terminal.
        return static_cast<u16>(static_cast<u8>(getchar()));

    case MMIO_TIMER_LO:
        // Lower 16 bits of the CPU cycle counter.
        return static_cast<u16>(cpu_.cycles & 0xFFFFu);

    case MMIO_TIMER_HI:
        // Upper 16 bits of the CPU cycle counter.
        return static_cast<u16>((cpu_.cycles >> 16) & 0xFFFFu);

    default:
        return 0;
    }
}

// ── MMIO write ────────────────────────────────────────────────────────────────
void Bus::mmio_write(u16 addr, u16 val) {
    switch (addr) {
    case MMIO_STDOUT_CHAR:
        // Print the lower byte as an ASCII character.
        putchar(static_cast<int>(val & 0xFF));
        fflush(stdout);
        break;

    case MMIO_STDOUT_NUM:
        // Print the 16-bit value as a signed decimal number followed by newline.
        printf("%d\n", static_cast<int>(static_cast<s16>(val)));
        fflush(stdout);
        break;

    default:
        break;  // writes to other MMIO addresses are silently ignored
    }
}
