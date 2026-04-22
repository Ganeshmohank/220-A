#pragma once
// =============================================================================
//  SPARTAN-16  –  Control Unit
// =============================================================================
//  The Control Unit drives the classic three-stage pipeline:
//
//    ┌─────────┐    ┌─────────┐    ┌─────────┐
//    │  FETCH  │ →  │ DECODE  │ →  │ EXECUTE │
//    └─────────┘    └─────────┘    └─────────┘
//
//  FETCH   : Read 4 bytes from memory at PC; advance PC by 4.
//  DECODE  : Extract opcode, rd, rs1, rs2/imm from the 32-bit word.
//  EXECUTE : Perform the operation (ALU / memory / control flow).
//
//  step() runs one complete F/D/E cycle.  It returns false when the CPU
//  has halted (HLT instruction executed or MAX_CYCLES reached).
// =============================================================================
#include "cpu.h"
#include "alu.h"
#include "bus.h"
#include <string>

class ControlUnit {
public:
    ControlUnit(CPUState& cpu, ALU& alu, Bus& bus)
        : cpu_(cpu), alu_(alu), bus_(bus) {}

    // Run one Fetch/Decode/Execute cycle.
    // If trace is true, print a disassembly line to stdout first.
    // Returns false when halted.
    bool step(bool trace = false);

private:
    CPUState& cpu_;
    ALU&      alu_;
    Bus&      bus_;

    // Pipeline stages
    u32         fetch();
    Instruction decode(u32 word);
    void        execute(const Instruction& instr);

    // Trace helper: print "  PC=0xXXXX | disasm | reg_changed=val  [flags]"
    void print_trace(u16 pc_before, u32 raw) const;
};
