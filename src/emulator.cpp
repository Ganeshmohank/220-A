// =============================================================================
//  SPARTAN-16  –  Emulator implementation
// =============================================================================
#include "emulator.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdexcept>

// ── Constructor ───────────────────────────────────────────────────────────────
//  Wire references: Bus needs Memory + CPUState; ControlUnit needs all three.
Emulator::Emulator()
    : cpu_()
    , mem_()
    , alu_()
    , bus_(mem_, cpu_)
    , cu_ (cpu_, alu_, bus_)
{
    cpu_.reset();
}

// ── reset() ───────────────────────────────────────────────────────────────────
void Emulator::reset() {
    cpu_.reset();
    mem_.reset();
}

// ── load(path) ────────────────────────────────────────────────────────────────
//  Binary format:
//    Bytes 0-3  : magic  "SP16"
//    Bytes 4-5  : load address  (u16 little-endian)
//    Bytes 6-7  : data length   (u16 little-endian, number of bytes after header)
//    Bytes 8+   : program bytes
bool Emulator::load(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        fprintf(stderr, "Error: cannot open '%s'\n", path.c_str());
        return false;
    }

    // Read entire file
    std::vector<u8> buf((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());

    if (buf.size() < BIN_HEADER_SIZE) {
        fprintf(stderr, "Error: '%s' is too small to contain a valid header\n",
                path.c_str());
        return false;
    }

    // Verify magic
    if (memcmp(buf.data(), BIN_MAGIC, 4) != 0) {
        fprintf(stderr, "Error: '%s' has invalid magic bytes (expected 'SP16')\n",
                path.c_str());
        return false;
    }

    u16 load_addr  = static_cast<u16>(buf[4]) | (static_cast<u16>(buf[5]) << 8);
    u16 data_len   = static_cast<u16>(buf[6]) | (static_cast<u16>(buf[7]) << 8);

    if (buf.size() < BIN_HEADER_SIZE + data_len) {
        fprintf(stderr, "Error: '%s' header claims %u bytes but file only has %zu\n",
                path.c_str(), data_len, buf.size() - BIN_HEADER_SIZE);
        return false;
    }

    // Copy program into RAM; reset CPU so PC starts at the load address
    reset();
    mem_.load(load_addr, buf.data() + BIN_HEADER_SIZE, data_len);
    cpu_.pc = load_addr;   // start execution at the load address

    printf("Loaded %u bytes at 0x%04X from '%s'\n",
           data_len, load_addr, path.c_str());
    return true;
}

// ── run(trace) ────────────────────────────────────────────────────────────────
void Emulator::run(bool trace) {
    if (trace) {
        printf("\n── Trace (PC | Instruction | Flags | Cycles) ──────────────────────\n");
    }

    while (!cpu_.halted && cpu_.cycles < MAX_CYCLES) {
        cu_.step(trace);
    }

    if (cpu_.cycles >= MAX_CYCLES && !cpu_.halted) {
        fprintf(stderr,
                "\nWarning: execution stopped after %llu cycles (runaway guard)\n",
                (unsigned long long)MAX_CYCLES);
    }

    if (trace) {
        printf("────────────────────────────────────────────────────────────────────\n");
    }

    printf("\nHalted after %llu instruction(s).\n", (unsigned long long)cpu_.cycles);
}

// ── dump_memory(start, end) ───────────────────────────────────────────────────
//  Classic hex + ASCII dump, 16 bytes per row.
void Emulator::dump_memory(u16 start_addr, u16 end_addr) const {
    const auto& raw = mem_.raw();

    printf("\n── Memory Dump  0x%04X – 0x%04X ───────────────────────────────────\n",
           start_addr, end_addr);

    for (u16 base = start_addr; base <= end_addr; base = static_cast<u16>(base + 16)) {
        printf("  %04X: ", static_cast<unsigned>(base));

        // Hex columns
        for (int col = 0; col < 16; ++col) {
            u16 addr = static_cast<u16>(base + col);
            if (addr <= end_addr)
                printf("%02X ", raw[addr]);
            else
                printf("   ");
            if (col == 7) printf(" ");  // visual separator at midpoint
        }

        // ASCII column
        printf(" |");
        for (int col = 0; col < 16; ++col) {
            u16 addr = static_cast<u16>(base + col);
            if (addr > end_addr) break;
            u8 byte = raw[addr];
            printf("%c", (byte >= 32 && byte < 127) ? static_cast<char>(byte) : '.');
        }
        printf("|\n");

        if (base > static_cast<u16>(0xFFFF - 16)) break;  // avoid u16 overflow
    }
    printf("────────────────────────────────────────────────────────────────────\n");
}
