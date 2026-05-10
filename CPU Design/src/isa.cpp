// =============================================================================
//  SPARTAN-16  –  ISA helper implementations (names, disassembler)
// =============================================================================
#include "isa.h"
#include <cstdio>
#include <cstring>
#include <string>

// ── Opcode → mnemonic string ─────────────────────────────────────────────────
const char* opcode_name(Opcode op) {
    switch (op) {
    case NOP:    return "NOP";
    case HLT:    return "HLT";
    case ADD:    return "ADD";
    case SUB:    return "SUB";
    case AND:    return "AND";
    case OR:     return "OR";
    case XOR:    return "XOR";
    case NOT:    return "NOT";
    case SHL:    return "SHL";
    case SHR:    return "SHR";
    case MUL:    return "MUL";
    case DIV:    return "DIV";
    case MOD:    return "MOD";
    case CMP:    return "CMP";
    case MOV:    return "MOV";
    case ADDI:   return "ADDI";
    case SUBI:   return "SUBI";
    case ANDI:   return "ANDI";
    case ORI:    return "ORI";
    case XORI:   return "XORI";
    case LI:     return "LI";
    case CMPI:   return "CMPI";
    case LOAD:   return "LOAD";
    case STORE:  return "STORE";
    case LOADB:  return "LOADB";
    case STOREB: return "STOREB";
    case JMP:    return "JMP";
    case JMPR:   return "JMPR";
    case JZ:     return "JZ";
    case JNZ:    return "JNZ";
    case JN:     return "JN";
    case JNN:    return "JNN";
    case JC:     return "JC";
    case JV:     return "JV";
    case CALL:   return "CALL";
    case CALLR:  return "CALLR";
    case RET:    return "RET";
    case PUSH:   return "PUSH";
    case POP:    return "POP";
    default:     return "???";
    }
}

// ── Register index → name string ─────────────────────────────────────────────
const char* reg_name(u8 reg) {
    static const char* names[] = {
        "R0","R1","R2","R3","R4","R5","R6","R7","SP","LR"
    };
    if (reg < NUM_REGS) return names[reg];
    return "??";
}

// ── One-line disassembler ────────────────────────────────────────────────────
//  Returns a human-readable string, e.g. "ADD R0, R1, R2" or "LOAD R3, [R1+4]"
std::string disassemble(u32 word) {
    Instruction i = decode_instruction(word);
    char buf[80];
    s16  soff = static_cast<s16>(i.imm);   // signed offset / immediate

    switch (i.opcode) {
    // ── no operands ──────────────────────────────────────────────────────────
    case NOP:
    case HLT:
    case RET:
        return opcode_name(i.opcode);

    // ── R-type: rd, rs1, rs2 ─────────────────────────────────────────────────
    case ADD: case SUB: case AND: case OR: case XOR:
    case SHL: case SHR: case MUL: case DIV: case MOD:
        snprintf(buf, sizeof(buf), "%-6s %s, %s, %s",
                 opcode_name(i.opcode), reg_name(i.rd),
                 reg_name(i.rs1), reg_name(rs2_of(i)));
        return buf;

    // ── NOT, MOV: rd, rs1 ────────────────────────────────────────────────────
    case NOT:
    case MOV:
        snprintf(buf, sizeof(buf), "%-6s %s, %s",
                 opcode_name(i.opcode), reg_name(i.rd), reg_name(i.rs1));
        return buf;

    // ── CMP: rs1, rs2 ────────────────────────────────────────────────────────
    case CMP:
        snprintf(buf, sizeof(buf), "CMP    %s, %s",
                 reg_name(i.rs1), reg_name(rs2_of(i)));
        return buf;

    // ── I-type ALU: rd, rs1, imm ─────────────────────────────────────────────
    case ADDI: case SUBI: case ANDI: case ORI: case XORI:
        snprintf(buf, sizeof(buf), "%-6s %s, %s, %d",
                 opcode_name(i.opcode), reg_name(i.rd),
                 reg_name(i.rs1), (int)soff);
        return buf;

    // ── LI: rd, imm ──────────────────────────────────────────────────────────
    case LI:
        snprintf(buf, sizeof(buf), "LI     %s, 0x%04X", reg_name(i.rd), i.imm);
        return buf;

    // ── CMPI: rs1, imm ───────────────────────────────────────────────────────
    case CMPI:
        snprintf(buf, sizeof(buf), "CMPI   %s, %d", reg_name(i.rs1), (int)soff);
        return buf;

    // ── LOAD / LOADB: rd, [rs1 + offset] ─────────────────────────────────────
    case LOAD:
    case LOADB:
        if (soff == 0)
            snprintf(buf, sizeof(buf), "%-6s %s, [%s]",
                     opcode_name(i.opcode), reg_name(i.rd), reg_name(i.rs1));
        else
            snprintf(buf, sizeof(buf), "%-6s %s, [%s%+d]",
                     opcode_name(i.opcode), reg_name(i.rd), reg_name(i.rs1), (int)soff);
        return buf;

    // ── STORE / STOREB: rd (src), [rs1 + offset] ─────────────────────────────
    case STORE:
    case STOREB:
        if (soff == 0)
            snprintf(buf, sizeof(buf), "%-6s %s, [%s]",
                     opcode_name(i.opcode), reg_name(i.rd), reg_name(i.rs1));
        else
            snprintf(buf, sizeof(buf), "%-6s %s, [%s%+d]",
                     opcode_name(i.opcode), reg_name(i.rd), reg_name(i.rs1), (int)soff);
        return buf;

    // ── Absolute branches: imm16 ──────────────────────────────────────────────
    case JMP: case JZ: case JNZ: case JN: case JNN: case JC: case JV: case CALL:
        snprintf(buf, sizeof(buf), "%-6s 0x%04X", opcode_name(i.opcode), i.imm);
        return buf;

    // ── Register-indirect branches ────────────────────────────────────────────
    case JMPR:
    case CALLR:
        snprintf(buf, sizeof(buf), "%-6s %s", opcode_name(i.opcode), reg_name(i.rs1));
        return buf;

    // ── Stack ─────────────────────────────────────────────────────────────────
    case PUSH:
        snprintf(buf, sizeof(buf), "PUSH   %s", reg_name(i.rd));
        return buf;
    case POP:
        snprintf(buf, sizeof(buf), "POP    %s", reg_name(i.rd));
        return buf;

    default:
        snprintf(buf, sizeof(buf), "??? (0x%08X)", word);
        return buf;
    }
}
