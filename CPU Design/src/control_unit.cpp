// =============================================================================
//  SPARTAN-16  –  Control Unit (Fetch / Decode / Execute)
// =============================================================================
#include "control_unit.h"
#include "isa.h"
#include <cstdio>
#include <stdexcept>
#include <string>

// =============================================================================
//  PUBLIC: step()
//  Runs one complete pipeline cycle.  Returns false when halted.
// =============================================================================
bool ControlUnit::step(bool trace) {
    if (cpu_.halted) return false;

    // ── FETCH ─────────────────────────────────────────────────────────────────
    u16 pc_before = cpu_.pc;
    u32 raw       = fetch();

    // ── DECODE ────────────────────────────────────────────────────────────────
    Instruction instr = decode(raw);

    // Optional trace line (printed after fetch & decode, before execute)
    if (trace) print_trace(pc_before, raw);

    // ── EXECUTE ───────────────────────────────────────────────────────────────
    execute(instr);

    cpu_.cycles++;
    return !cpu_.halted;
}

// =============================================================================
//  FETCH
//  Read 4 bytes from memory at PC and advance PC by 4.
// =============================================================================
u32 ControlUnit::fetch() {
    u32 word = bus_.fetch32(cpu_.pc);
    cpu_.pc  = static_cast<u16>(cpu_.pc + 4);
    return word;
}

// =============================================================================
//  DECODE
//  Split the 32-bit instruction word into its fields.
// =============================================================================
Instruction ControlUnit::decode(u32 word) {
    return decode_instruction(word);
}

// =============================================================================
//  EXECUTE
//  Dispatch on opcode and carry out the operation.
// =============================================================================
void ControlUnit::execute(const Instruction& instr) {

    // Handy aliases
    const u8  rd  = instr.rd;
    const u8  rs1 = instr.rs1;
    const u8  rs2 = rs2_of(instr);       // bits [15:12] of imm field (R-type)
    const u16 imm = instr.imm;           // 16-bit immediate (I-type)

    // rs1 / rs2 values read from register file
    u16 a = cpu_.reg(rs1);
    u16 b = cpu_.reg(rs2);

    switch (instr.opcode) {

    // ── Misc ──────────────────────────────────────────────────────────────────
    case NOP:
        break;

    case HLT:
        cpu_.halted = true;
        break;

    // ── R-type ALU ────────────────────────────────────────────────────────────
    case ADD: { auto r = alu_.add(a, b);           cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case SUB: { auto r = alu_.sub(a, b);           cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case AND: { auto r = alu_.bitwise_and(a, b);   cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case OR:  { auto r = alu_.bitwise_or (a, b);   cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case XOR: { auto r = alu_.bitwise_xor(a, b);   cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case NOT: { auto r = alu_.bitwise_not(a);       cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case SHL: { auto r = alu_.shl(a, b);            cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case SHR: { auto r = alu_.shr(a, b);            cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case MUL: { auto r = alu_.mul(a, b);            cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case DIV: { auto r = alu_.div_op(a, b);         cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case MOD: { auto r = alu_.mod_op(a, b);         cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }

    case CMP: {
        // Compare: set flags based on rs1 - rs2, but do NOT store the result.
        auto r = alu_.sub(a, b);
        cpu_.flags = r.flags;
        break;
    }

    case MOV: {
        // Copy rs1 into rd; update Z and N flags.
        cpu_.regs[rd] = a;
        cpu_.flags    = (a == 0) ? FLAG_Z : 0u;
        if (a & 0x8000u) cpu_.flags |= FLAG_N;
        break;
    }

    // ── I-type ALU ────────────────────────────────────────────────────────────
    //  The immediate is treated as an unsigned 16-bit pattern; two's-complement
    //  arithmetic naturally handles negative values (e.g. 0xFFFF = -1).
    case ADDI: { auto r = alu_.add(a, imm);          cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case SUBI: { auto r = alu_.sub(a, imm);          cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case ANDI: { auto r = alu_.bitwise_and(a, imm);  cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case ORI:  { auto r = alu_.bitwise_or (a, imm);  cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }
    case XORI: { auto r = alu_.bitwise_xor(a, imm);  cpu_.regs[rd] = r.value; cpu_.flags = r.flags; break; }

    case LI: {
        // Load immediate: rd = imm (bit pattern preserved, no sign extension).
        cpu_.regs[rd] = imm;
        cpu_.flags    = (imm == 0) ? FLAG_Z : 0u;
        if (imm & 0x8000u) cpu_.flags |= FLAG_N;
        break;
    }

    case CMPI: {
        // Compare immediate: flags = rs1 - imm, no store.
        auto r = alu_.sub(a, imm);
        cpu_.flags = r.flags;
        break;
    }

    // ── Memory ────────────────────────────────────────────────────────────────
    //  Effective address = rs1 + sign_ext(imm16).
    //  Because addresses are u16 and imm is also u16, two's-complement addition
    //  already handles negative offsets (e.g. 0xFFFC = -4).
    case LOAD: {
        u16 addr      = static_cast<u16>(cpu_.regs[rs1] + imm);
        cpu_.regs[rd] = bus_.readWord(addr);
        break;
    }
    case STORE: {
        // rd field carries the *source* register (the value to write).
        u16 addr = static_cast<u16>(cpu_.regs[rs1] + imm);
        bus_.writeWord(addr, cpu_.regs[rd]);
        break;
    }
    case LOADB: {
        u16 addr      = static_cast<u16>(cpu_.regs[rs1] + imm);
        cpu_.regs[rd] = static_cast<u16>(bus_.readByte(addr));
        break;
    }
    case STOREB: {
        u16 addr = static_cast<u16>(cpu_.regs[rs1] + imm);
        bus_.writeByte(addr, static_cast<u8>(cpu_.regs[rd] & 0xFF));
        break;
    }

    // ── Control flow ──────────────────────────────────────────────────────────
    case JMP:  { cpu_.pc = imm;             break; }
    case JMPR: { cpu_.pc = cpu_.regs[rs1];  break; }
    case JZ:   { if ( cpu_.flag_z()) cpu_.pc = imm; break; }
    case JNZ:  { if (!cpu_.flag_z()) cpu_.pc = imm; break; }
    case JN:   { if ( cpu_.flag_n()) cpu_.pc = imm; break; }
    case JNN:  { if (!cpu_.flag_n()) cpu_.pc = imm; break; }
    case JC:   { if ( cpu_.flag_c()) cpu_.pc = imm; break; }
    case JV:   { if ( cpu_.flag_v()) cpu_.pc = imm; break; }

    case CALL: {
        // Save the already-advanced PC (= address of next instruction) in LR.
        cpu_.regs[REG_LR] = cpu_.pc;
        cpu_.pc            = imm;
        break;
    }
    case CALLR: {
        cpu_.regs[REG_LR] = cpu_.pc;
        cpu_.pc            = cpu_.regs[rs1];
        break;
    }
    case RET: {
        cpu_.pc = cpu_.regs[REG_LR];
        break;
    }

    // ── Stack ─────────────────────────────────────────────────────────────────
    case PUSH: {
        // Decrement SP first (stack grows downward), then write.
        cpu_.regs[REG_SP] = static_cast<u16>(cpu_.regs[REG_SP] - 2);
        bus_.writeWord(cpu_.regs[REG_SP], cpu_.regs[rd]);
        break;
    }
    case POP: {
        // Read from SP, then increment.
        cpu_.regs[rd]     = bus_.readWord(cpu_.regs[REG_SP]);
        cpu_.regs[REG_SP] = static_cast<u16>(cpu_.regs[REG_SP] + 2);
        break;
    }

    default: {
        char msg[64];
        snprintf(msg, sizeof(msg),
                 "Illegal opcode 0x%02X at PC=0x%04X",
                 static_cast<unsigned>(instr.opcode),
                 static_cast<unsigned>(cpu_.pc - 4));
        throw std::runtime_error(msg);
    }
    } // end switch
}

// =============================================================================
//  TRACE helper
// =============================================================================
void ControlUnit::print_trace(u16 pc_before, u32 raw) const {
    std::string dis = disassemble(raw);
    printf("  0x%04X | %-30s | flags=%s  cycles=%-8llu\n",
           static_cast<unsigned>(pc_before),
           dis.c_str(),
           cpu_.flags_str().c_str(),
           (unsigned long long)cpu_.cycles);
}
