#pragma once
// =============================================================================
//  SPARTAN-16  –  Two-pass Assembler
// =============================================================================
//  Pass 1: scan every line, record label→address in the symbol table,
//          advance the program counter by the byte size of each statement.
//  Pass 2: re-scan every line, resolve labels, and emit the binary output.
//
//  Output format  (written to the .bin file):
//    Bytes 0-3 : magic  "SP16"
//    Bytes 4-5 : load address (u16 LE, default 0x0000 or value of first .org)
//    Bytes 6-7 : data length  (u16 LE)
//    Bytes 8+  : assembled bytes
//
//  Assembly syntax  (see docs/isa.md for complete reference):
//    ; comment                      – everything from ; to end of line ignored
//    label:                         – defines label at current address
//    label: INSTRUCTION ops…        – label and instruction on same line
//    .org  <addr>                   – set current address
//    .word <val>                    – emit 16-bit little-endian word
//    .byte <val>                    – emit one byte
//    .string "text\n"               – emit null-terminated string (escape seqs ok)
//    .align <n>                     – advance to next multiple of n bytes
// =============================================================================
#include "isa.h"
#include <map>
#include <string>
#include <vector>

class Assembler {
public:
    // Assemble `infile` and write the binary to `outfile`.
    // Returns true on success.  On error, call error() for a description.
    bool assemble(const std::string& infile, const std::string& outfile);

    const std::string& error() const { return error_; }

private:
    // ── State ─────────────────────────────────────────────────────────────────
    std::map<std::string, u16> symbols_;  // label → address
    std::vector<u8>            output_;   // assembled bytes
    u16                        org_   = 0;  // load address (.org sets this)
    u16                        pc_    = 0;  // current address during assembly
    int                        line_num_ = 0;
    std::string                error_;

    // ── Passes ────────────────────────────────────────────────────────────────
    bool pass1(const std::vector<std::string>& lines);
    bool pass2(const std::vector<std::string>& lines);

    // ── Per-line processor ────────────────────────────────────────────────────
    //  emit=false → only advance pc_ (pass 1)
    //  emit=true  → advance pc_ AND write bytes to output_ (pass 2)
    bool process_line(const std::string& raw, bool emit);

    // ── Directive handlers ────────────────────────────────────────────────────
    bool handle_directive(const std::string& line, bool emit);

    // ── Instruction assembly ──────────────────────────────────────────────────
    bool assemble_instruction(const std::string& mnemonic,
                               const std::vector<std::string>& operands,
                               bool emit);

    // ── Operand parsers ───────────────────────────────────────────────────────
    int    parse_reg(const std::string& s);           // returns 0-9, or -1
    u16    resolve_imm(const std::string& s);         // number or label lookup
    bool   parse_mem(const std::string& s,            // "[Rs]" or "[Rs±n]"
                     int& base_reg, s16& offset);

    // ── Emit helpers ──────────────────────────────────────────────────────────
    void emit8 (u8  byte);
    void emit16(u16 word);      // little-endian
    void emit32(u32 word);      // little-endian

    // ── String utilities ──────────────────────────────────────────────────────
    static std::string strip_comment(const std::string& line);
    static std::string trim        (const std::string& s);
    static std::string to_upper    (const std::string& s);
    static bool        is_identifier(const std::string& s);
    static s32         parse_literal(const std::string& s); // number literal

    // ── Error helper ──────────────────────────────────────────────────────────
    bool set_error(const std::string& msg);
};
