// =============================================================================
//  SPARTAN-16  –  Two-pass Assembler implementation
// =============================================================================
#include "assembler.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

// =============================================================================
//  PUBLIC: assemble()
// =============================================================================
bool Assembler::assemble(const std::string& infile, const std::string& outfile) {
    // ── Read source file ──────────────────────────────────────────────────────
    std::ifstream in(infile);
    if (!in) return set_error("cannot open '" + infile + "'");

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);

    // ── Reset state ───────────────────────────────────────────────────────────
    symbols_.clear();
    output_.clear();
    org_ = 0;
    pc_  = 0;

    // ── Pass 1: collect all labels ────────────────────────────────────────────
    if (!pass1(lines)) return false;

    // ── Pass 2: emit binary ───────────────────────────────────────────────────
    pc_ = org_;
    output_.clear();
    if (!pass2(lines)) return false;

    // ── Write output file  (8-byte header + data) ────────────────────────────
    std::ofstream out(outfile, std::ios::binary);
    if (!out) return set_error("cannot write '" + outfile + "'");

    u16 data_len = static_cast<u16>(output_.size());

    out.write(reinterpret_cast<const char*>(BIN_MAGIC), 4);
    out.put(static_cast<char>(org_ & 0xFF));
    out.put(static_cast<char>(org_ >> 8));
    out.put(static_cast<char>(data_len & 0xFF));
    out.put(static_cast<char>(data_len >> 8));
    out.write(reinterpret_cast<const char*>(output_.data()), output_.size());

    printf("Assembled %zu bytes → '%s'  (load addr 0x%04X)\n",
           output_.size(), outfile.c_str(), org_);
    return true;
}

// =============================================================================
//  PASS 1  – record labels and advance PC
// =============================================================================
bool Assembler::pass1(const std::vector<std::string>& lines) {
    pc_ = org_;
    for (size_t i = 0; i < lines.size(); ++i) {
        line_num_ = static_cast<int>(i) + 1;
        if (!process_line(lines[i], false)) return false;
    }
    return true;
}

// =============================================================================
//  PASS 2  – emit machine code
// =============================================================================
bool Assembler::pass2(const std::vector<std::string>& lines) {
    pc_ = org_;
    for (size_t i = 0; i < lines.size(); ++i) {
        line_num_ = static_cast<int>(i) + 1;
        if (!process_line(lines[i], true)) return false;
    }
    return true;
}

// =============================================================================
//  PER-LINE PROCESSING
// =============================================================================
bool Assembler::process_line(const std::string& raw, bool emit) {
    std::string line = strip_comment(raw);
    line = trim(line);
    if (line.empty()) return true;

    // ── Check for a label at the start of the line ────────────────────────────
    //  A label is an identifier immediately followed by ':'.
    //  Everything after the ':' is processed as the rest of the line.
    {
        size_t i = 0;
        size_t start = i;
        while (i < line.size() && (std::isalnum((u8)line[i]) || line[i] == '_')) ++i;
        if (i > start && i < line.size() && line[i] == ':') {
            std::string lname = line.substr(start, i - start);
            if (!emit) {
                // Only record in pass 1 (or verify not duplicate)
                symbols_[lname] = pc_;
            }
            line = trim(line.substr(i + 1));
            if (line.empty()) return true;
        }
    }

    // ── Assembler directives (.org, .word, .byte, .string, .align) ────────────
    if (!line.empty() && line[0] == '.') {
        return handle_directive(line, emit);
    }

    // ── Instruction ───────────────────────────────────────────────────────────
    // Split mnemonic from operands
    std::istringstream ss(line);
    std::string mnemonic;
    ss >> mnemonic;
    mnemonic = to_upper(mnemonic);

    // Collect remainder as operand string, then split on ','
    std::string rest;
    std::getline(ss, rest);
    rest = trim(rest);

    std::vector<std::string> operands;
    if (!rest.empty()) {
        // Split by ',' but not inside brackets
        std::string cur;
        int depth = 0;
        for (char c : rest) {
            if (c == '[') { depth++; cur += c; }
            else if (c == ']') { depth--; cur += c; }
            else if (c == ',' && depth == 0) {
                operands.push_back(trim(cur));
                cur.clear();
            } else {
                cur += c;
            }
        }
        if (!trim(cur).empty()) operands.push_back(trim(cur));
    }

    return assemble_instruction(mnemonic, operands, emit);
}

// =============================================================================
//  DIRECTIVE HANDLER
// =============================================================================
bool Assembler::handle_directive(const std::string& line, bool emit) {
    std::istringstream ss(line);
    std::string name;
    ss >> name;
    name = to_upper(name);

    if (name == ".ORG") {
        std::string tok; ss >> tok;
        u16 addr = static_cast<u16>(parse_literal(tok));
        org_ = addr;
        pc_  = addr;
        return true;
    }

    if (name == ".WORD") {
        std::string tok; ss >> tok;
        if (emit) emit16(static_cast<u16>(parse_literal(tok)));
        pc_ = static_cast<u16>(pc_ + 2);
        return true;
    }

    if (name == ".BYTE") {
        std::string tok; ss >> tok;
        if (emit) emit8(static_cast<u8>(parse_literal(tok) & 0xFF));
        pc_ = static_cast<u16>(pc_ + 1);
        return true;
    }

    if (name == ".ALIGN") {
        std::string tok; ss >> tok;
        u16 n = static_cast<u16>(parse_literal(tok));
        if (n > 0) {
            u16 pad = static_cast<u16>((n - (pc_ % n)) % n);
            for (u16 i = 0; i < pad; ++i) {
                if (emit) emit8(0);
                pc_ = static_cast<u16>(pc_ + 1);
            }
        }
        return true;
    }

    if (name == ".STRING") {
        // Find the opening quote
        auto qstart = line.find('"');
        if (qstart == std::string::npos)
            return set_error("line " + std::to_string(line_num_) +
                             ": .string missing opening '\"'");
        auto qend = line.rfind('"');
        if (qend == qstart)
            return set_error("line " + std::to_string(line_num_) +
                             ": .string missing closing '\"'");

        std::string content = line.substr(qstart + 1, qend - qstart - 1);

        // Process escape sequences
        for (size_t i = 0; i < content.size(); ++i) {
            char c = content[i];
            if (c == '\\' && i + 1 < content.size()) {
                char next = content[++i];
                switch (next) {
                case 'n':  c = '\n'; break;
                case 't':  c = '\t'; break;
                case 'r':  c = '\r'; break;
                case '0':  c = '\0'; break;
                case '\\': c = '\\'; break;
                case '\'': c = '\''; break;
                case '"':  c = '"';  break;
                default:   c = next; break;
                }
            }
            if (emit) emit8(static_cast<u8>(c));
            pc_ = static_cast<u16>(pc_ + 1);
        }
        // Null terminator
        if (emit) emit8(0);
        pc_ = static_cast<u16>(pc_ + 1);
        return true;
    }

    return set_error("line " + std::to_string(line_num_) +
                     ": unknown directive '" + name + "'");
}

// =============================================================================
//  INSTRUCTION ASSEMBLER
// =============================================================================
bool Assembler::assemble_instruction(const std::string& mn,
                                      const std::vector<std::string>& ops,
                                      bool emit) {
    // Pass 1 (emit=false): every instruction is exactly 4 bytes.
    // Skip full operand parsing so forward-reference labels don't cause spurious errors.
    if (!emit) {
        pc_ = static_cast<u16>(pc_ + 4);
        return true;
    }

    // Helper lambdas
    auto need = [&](size_t n) -> bool {
        if (ops.size() < n)
            return !set_error("line " + std::to_string(line_num_) +
                              ": '" + mn + "' expects " + std::to_string(n) +
                              " operand(s), got " + std::to_string(ops.size()));
        return true;
    };
    auto reg = [&](size_t idx) -> int {
        int r = parse_reg(ops[idx]);
        if (r < 0) set_error("line " + std::to_string(line_num_) +
                              ": '" + ops[idx] + "' is not a valid register");
        return r;
    };
    auto imm = [&](size_t idx) -> u16 {
        return resolve_imm(ops[idx]);
    };

    // All instructions advance PC by 4 bytes
    auto emit_word = [&](u32 word) {
        if (emit) emit32(word);
        pc_ = static_cast<u16>(pc_ + 4);
    };

    // ── No operands ───────────────────────────────────────────────────────────
    if (mn == "NOP")  { emit_word(encode_instruction(NOP, 0,0,0)); return true; }
    if (mn == "HLT")  { emit_word(encode_instruction(HLT, 0,0,0)); return true; }
    if (mn == "RET")  { emit_word(encode_instruction(RET, 0,0,0)); return true; }

    // ── R-type: rd, rs1, rs2 ─────────────────────────────────────────────────
    auto rtype3 = [&](Opcode op) -> bool {
        if (!need(3)) return false;
        int rd = reg(0), rs1 = reg(1), rs2 = reg(2);
        if (rd<0||rs1<0||rs2<0) return false;
        emit_word(encode_rtype(op, (u8)rd, (u8)rs1, (u8)rs2));
        return true;
    };
    if (mn == "ADD") return rtype3(ADD);
    if (mn == "SUB") return rtype3(SUB);
    if (mn == "AND") return rtype3(AND);
    if (mn == "OR")  return rtype3(OR);
    if (mn == "XOR") return rtype3(XOR);
    if (mn == "SHL") return rtype3(SHL);
    if (mn == "SHR") return rtype3(SHR);
    if (mn == "MUL") return rtype3(MUL);
    if (mn == "DIV") return rtype3(DIV);
    if (mn == "MOD") return rtype3(MOD);

    // ── NOT: rd, rs1 ─────────────────────────────────────────────────────────
    if (mn == "NOT") {
        if (!need(2)) return false;
        int rd = reg(0), rs1 = reg(1);
        if (rd<0||rs1<0) return false;
        emit_word(encode_rtype(NOT, (u8)rd, (u8)rs1, 0));
        return true;
    }

    // ── MOV: rd, rs1 ─────────────────────────────────────────────────────────
    if (mn == "MOV") {
        if (!need(2)) return false;
        int rd = reg(0), rs1 = reg(1);
        if (rd<0||rs1<0) return false;
        emit_word(encode_rtype(MOV, (u8)rd, (u8)rs1, 0));
        return true;
    }

    // ── CMP: rs1, rs2 (no destination) ───────────────────────────────────────
    if (mn == "CMP") {
        if (!need(2)) return false;
        int rs1 = reg(0), rs2 = reg(1);
        if (rs1<0||rs2<0) return false;
        emit_word(encode_rtype(CMP, 0, (u8)rs1, (u8)rs2));
        return true;
    }

    // ── I-type: rd, rs1, imm ─────────────────────────────────────────────────
    auto itype3 = [&](Opcode op) -> bool {
        if (!need(3)) return false;
        int rd = reg(0), rs1 = reg(1);
        if (rd<0||rs1<0) return false;
        u16 v = imm(2);
        emit_word(encode_instruction(op, (u8)rd, (u8)rs1, v));
        return true;
    };
    if (mn == "ADDI") return itype3(ADDI);
    if (mn == "SUBI") return itype3(SUBI);
    if (mn == "ANDI") return itype3(ANDI);
    if (mn == "ORI")  return itype3(ORI);
    if (mn == "XORI") return itype3(XORI);

    // ── LI: rd, imm ──────────────────────────────────────────────────────────
    if (mn == "LI") {
        if (!need(2)) return false;
        int rd = reg(0);
        if (rd < 0) return false;
        u16 v = imm(1);
        emit_word(encode_instruction(LI, (u8)rd, 0, v));
        return true;
    }

    // ── CMPI: rs1, imm ───────────────────────────────────────────────────────
    if (mn == "CMPI") {
        if (!need(2)) return false;
        int rs1 = reg(0);
        if (rs1 < 0) return false;
        u16 v = imm(1);
        emit_word(encode_instruction(CMPI, 0, (u8)rs1, v));
        return true;
    }

    // ── LOAD / LOADB: rd, [rs1 + offset] ─────────────────────────────────────
    auto load_op = [&](Opcode op) -> bool {
        if (!need(2)) return false;
        int rd = reg(0);
        if (rd < 0) return false;
        int base; s16 off;
        if (!parse_mem(ops[1], base, off)) return false;
        emit_word(encode_instruction(op, (u8)rd, (u8)base, (u16)off));
        return true;
    };
    if (mn == "LOAD")  return load_op(LOAD);
    if (mn == "LOADB") return load_op(LOADB);

    // ── STORE / STOREB: rs_data, [rs_base + offset] ───────────────────────────
    //  The rd field holds the source (data) register.
    auto store_op = [&](Opcode op) -> bool {
        if (!need(2)) return false;
        int src = reg(0);  // value to store
        if (src < 0) return false;
        int base; s16 off;
        if (!parse_mem(ops[1], base, off)) return false;
        emit_word(encode_instruction(op, (u8)src, (u8)base, (u16)off));
        return true;
    };
    if (mn == "STORE")  return store_op(STORE);
    if (mn == "STOREB") return store_op(STOREB);

    // ── Absolute branches: imm16 (label or address) ───────────────────────────
    auto branch = [&](Opcode op) -> bool {
        if (!need(1)) return false;
        // If the operand is a register, use register-indirect variant
        int r = parse_reg(ops[0]);
        if (r >= 0) {
            Opcode rop = (op == JMP) ? JMPR : (op == CALL ? CALLR : op);
            emit_word(encode_instruction(rop, 0, (u8)r, 0));
        } else {
            u16 target = imm(0);
            emit_word(encode_instruction(op, 0, 0, target));
        }
        return true;
    };
    if (mn == "JMP")  return branch(JMP);
    if (mn == "JZ")   { if(!need(1)) return false; emit_word(encode_instruction(JZ,  0,0,imm(0))); return true; }
    if (mn == "JNZ")  { if(!need(1)) return false; emit_word(encode_instruction(JNZ, 0,0,imm(0))); return true; }
    if (mn == "JN")   { if(!need(1)) return false; emit_word(encode_instruction(JN,  0,0,imm(0))); return true; }
    if (mn == "JNN")  { if(!need(1)) return false; emit_word(encode_instruction(JNN, 0,0,imm(0))); return true; }
    if (mn == "JC")   { if(!need(1)) return false; emit_word(encode_instruction(JC,  0,0,imm(0))); return true; }
    if (mn == "JV")   { if(!need(1)) return false; emit_word(encode_instruction(JV,  0,0,imm(0))); return true; }
    if (mn == "CALL") return branch(CALL);

    // ── JMPR / CALLR: rs1 ────────────────────────────────────────────────────
    if (mn == "JMPR") {
        if (!need(1)) return false;
        int rs1 = reg(0); if(rs1<0) return false;
        emit_word(encode_instruction(JMPR, 0, (u8)rs1, 0));
        return true;
    }
    if (mn == "CALLR") {
        if (!need(1)) return false;
        int rs1 = reg(0); if(rs1<0) return false;
        emit_word(encode_instruction(CALLR, 0, (u8)rs1, 0));
        return true;
    }

    // ── PUSH: rs (source) ─────────────────────────────────────────────────────
    if (mn == "PUSH") {
        if (!need(1)) return false;
        int rs = reg(0); if(rs<0) return false;
        emit_word(encode_instruction(PUSH, (u8)rs, 0, 0));
        return true;
    }

    // ── POP: rd (destination) ─────────────────────────────────────────────────
    if (mn == "POP") {
        if (!need(1)) return false;
        int rd = reg(0); if(rd<0) return false;
        emit_word(encode_instruction(POP, (u8)rd, 0, 0));
        return true;
    }

    return set_error("line " + std::to_string(line_num_) +
                     ": unknown mnemonic '" + mn + "'");
}

// =============================================================================
//  REGISTER PARSER
// =============================================================================
int Assembler::parse_reg(const std::string& raw) {
    std::string s = to_upper(trim(raw));
    if (s == "SP") return REG_SP;
    if (s == "LR") return REG_LR;
    if (s.size() >= 2 && s[0] == 'R' && std::isdigit((u8)s[1])) {
        int n = std::stoi(s.substr(1));
        if (n >= 0 && n < NUM_REGS) return n;
    }
    return -1;  // not a register
}

// =============================================================================
//  IMMEDIATE / LABEL RESOLVER
// =============================================================================
u16 Assembler::resolve_imm(const std::string& raw) {
    std::string s = trim(raw);
    // First, try to parse as a number literal
    try {
        s32 v = parse_literal(s);
        return static_cast<u16>(v);   // truncate / two's-complement
    } catch (...) {}

    // Otherwise look up as a label
    auto it = symbols_.find(s);
    if (it != symbols_.end()) return it->second;

    set_error("line " + std::to_string(line_num_) +
              ": undefined symbol '" + s + "'");
    return 0;
}

// =============================================================================
//  MEMORY OPERAND PARSER   "[Rs]"  or  "[Rs + imm]"  or  "[Rs - imm]"
// =============================================================================
bool Assembler::parse_mem(const std::string& raw, int& base_reg, s16& offset) {
    std::string s = trim(raw);
    if (s.empty() || s.front() != '[' || s.back() != ']')
        return !set_error("line " + std::to_string(line_num_) +
                          ": expected memory operand '[Rs+imm]', got '" + raw + "'");

    std::string inner = trim(s.substr(1, s.size() - 2));

    // Look for '+' or '-' that separates register from offset
    // (skip past first character to avoid matching unary '-' on register name)
    size_t plus_pos  = inner.find('+');
    size_t minus_pos = inner.find('-', 1);  // start at 1 to skip any leading sign

    if (plus_pos != std::string::npos) {
        base_reg = parse_reg(trim(inner.substr(0, plus_pos)));
        offset   = static_cast<s16>(parse_literal(trim(inner.substr(plus_pos + 1))));
    } else if (minus_pos != std::string::npos) {
        base_reg = parse_reg(trim(inner.substr(0, minus_pos)));
        offset   = static_cast<s16>(-parse_literal(trim(inner.substr(minus_pos + 1))));
    } else {
        base_reg = parse_reg(inner);
        offset   = 0;
    }

    if (base_reg < 0)
        return !set_error("line " + std::to_string(line_num_) +
                          ": invalid base register in memory operand '" + raw + "'");
    return true;
}

// =============================================================================
//  EMIT HELPERS
// =============================================================================
void Assembler::emit8(u8 byte) {
    output_.push_back(byte);
}

void Assembler::emit16(u16 word) {
    output_.push_back(static_cast<u8>(word & 0xFF));
    output_.push_back(static_cast<u8>(word >> 8));
}

void Assembler::emit32(u32 word) {
    output_.push_back(static_cast<u8>( word        & 0xFF));
    output_.push_back(static_cast<u8>((word >>  8) & 0xFF));
    output_.push_back(static_cast<u8>((word >> 16) & 0xFF));
    output_.push_back(static_cast<u8>((word >> 24) & 0xFF));
}

// =============================================================================
//  STRING UTILITIES
// =============================================================================
std::string Assembler::strip_comment(const std::string& line) {
    bool in_string = false;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '"') in_string = !in_string;
        if (!in_string && line[i] == ';')
            return line.substr(0, i);
    }
    return line;
}

std::string Assembler::trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return {};
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::string Assembler::to_upper(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = static_cast<char>(std::toupper((u8)c));
    return r;
}

bool Assembler::is_identifier(const std::string& s) {
    if (s.empty()) return false;
    if (!std::isalpha((u8)s[0]) && s[0] != '_') return false;
    for (char c : s)
        if (!std::isalnum((u8)c) && c != '_') return false;
    return true;
}

// Parse a numeric literal: decimal, 0x hex, 0b binary, or 'c' char literal.
s32 Assembler::parse_literal(const std::string& raw) {
    std::string s = trim(raw);
    if (s.empty()) throw std::runtime_error("empty literal");

    // Character literal  'x'
    if (s.size() >= 3 && s.front() == '\'' && s.back() == '\'') {
        std::string inner = s.substr(1, s.size() - 2);
        if (inner == "\\n")  return '\n';
        if (inner == "\\t")  return '\t';
        if (inner == "\\r")  return '\r';
        if (inner == "\\0")  return '\0';
        if (inner == "\\\\") return '\\';
        if (inner == "\\'")  return '\'';
        if (inner.size() == 1) return static_cast<u8>(inner[0]);
        throw std::runtime_error("invalid char literal: " + s);
    }

    // Hex  0x…
    if (s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        return static_cast<s32>(std::stoul(s.substr(2), nullptr, 16));

    // Binary  0b…
    if (s.size() > 2 && s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))
        return static_cast<s32>(std::stoul(s.substr(2), nullptr, 2));

    // Signed decimal
    return std::stoi(s);
}

// =============================================================================
//  ERROR HELPER
// =============================================================================
bool Assembler::set_error(const std::string& msg) {
    error_ = msg;
    fprintf(stderr, "Assembler error: %s\n", msg.c_str());
    return false;
}
