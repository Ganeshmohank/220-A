#pragma once
// =============================================================================
//  SPARTAN-16  –  Shared type aliases, memory-map constants, and CPU defaults
// =============================================================================
#include <cstdint>
#include <string>

// ── Convenient integer type aliases ──────────────────────────────────────────
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

// ── Memory map ────────────────────────────────────────────────────────────────
//   0x0000 – 0xEFFF   RAM  (61 440 bytes, general-purpose)
//   0xF000 – 0xFFFF   Memory-mapped I/O region
static constexpr u32 MEM_SIZE  = 0x10000u;   // 64 KB total address space
static constexpr u16 RAM_END   = 0xEFFF;      // last byte of RAM (inclusive)
static constexpr u16 MMIO_BASE = 0xF000;      // first MMIO address

// ── MMIO register addresses ───────────────────────────────────────────────────
//  Writing to STDOUT_CHAR outputs the lower byte as an ASCII character.
//  Writing to STDOUT_NUM  outputs the 16-bit value as a signed decimal + '\n'.
//  Reading  STDIN         blocks for one character from stdin.
//  Reading  TIMER_LO/HI  returns the current CPU cycle count.
static constexpr u16 MMIO_STDOUT_CHAR = 0xF000;
static constexpr u16 MMIO_STDIN       = 0xF001;
static constexpr u16 MMIO_STDOUT_NUM  = 0xF002;
static constexpr u16 MMIO_TIMER_LO   = 0xF004;  // cycle[15:0]
static constexpr u16 MMIO_TIMER_HI   = 0xF006;  // cycle[31:16]

// ── CPU defaults ──────────────────────────────────────────────────────────────
static constexpr u16 RESET_VECTOR    = 0x0000;   // PC on power-on / reset
static constexpr u16 STACK_INIT_ADDR = 0xEFFE;   // SP on power-on / reset

// ── Safety limit ─────────────────────────────────────────────────────────────
static constexpr u64 MAX_CYCLES = 10'000'000ULL;  // halt if exceeded (runaway guard)

// ── Binary file header magic ──────────────────────────────────────────────────
// Layout: [4 B magic] [2 B load_addr LE] [2 B byte_count LE] [data…]
static constexpr u8  BIN_MAGIC[4] = {'S','P','1','6'};
static constexpr u32 BIN_HEADER_SIZE = 8;
