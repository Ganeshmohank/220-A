#pragma once
// =============================================================================
//  SPARTAN-16  –  Emulator (top-level system integration)
// =============================================================================
//  The Emulator owns and wires all hardware components:
//
//    Memory  ──► Bus ──► ControlUnit
//    CPUState ──►─┘         │
//    ALU ────────────────────┘
//
//  Public API used by main():
//    load(path)          – load a .bin file into RAM
//    run(trace)          – run until HLT or MAX_CYCLES
//    dump_memory(s, e)   – hex-dump memory range [s, e]
//    reset()             – return to power-on state
// =============================================================================
#include "memory.h"
#include "cpu.h"
#include "alu.h"
#include "bus.h"
#include "control_unit.h"
#include <string>

class Emulator {
public:
    Emulator();

    // Load a SPARTAN-16 binary (with 8-byte header) from disk.
    // Returns true on success; prints an error and returns false on failure.
    bool load(const std::string& path);

    // Run the loaded program.
    //  trace  – if true, print a disassembly line for every instruction
    void run(bool trace = false);

    // Print a hex + ASCII dump of memory in the range [start_addr, end_addr].
    void dump_memory(u16 start_addr, u16 end_addr) const;

    // Reset CPU and RAM to power-on state.
    void reset();

    // Expose CPU state for main() to print register file after run.
    const CPUState& cpu() const { return cpu_; }

private:
    CPUState     cpu_;
    Memory       mem_;
    ALU          alu_;
    Bus          bus_;   // constructed with references to mem_ and cpu_
    ControlUnit  cu_;    // constructed with references to cpu_, alu_, bus_
};
