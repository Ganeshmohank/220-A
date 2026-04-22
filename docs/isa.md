# SPARTAN-16 — Instruction Set Architecture Reference

## Overview

SPARTAN-16 is an educational 16-bit RISC-style CPU with:
- **16-bit** data path and address space (64 KB)
- **10 registers**: R0–R7 (general purpose), SP (R8), LR (R9)
- **32-bit fixed-width instructions** (little-endian in memory)
- **35 opcodes** covering ALU, memory, control flow, and stack operations
- **Memory-mapped I/O** for terminal input/output and a hardware timer

---

## Registers

| Name | Index | Purpose |
|------|-------|---------|
| R0–R7 | 0–7 | General-purpose (16-bit each) |
| SP | 8 | Stack Pointer – auto-decremented by PUSH, incremented by POP |
| LR | 9 | Link Register – holds return address after a CALL |
| PC | — | Program Counter (not directly addressable) |
| FLAGS | — | Status flags: Z N C V (see below) |

### Status Flags

| Flag | Bit | Meaning |
|------|-----|---------|
| Z | 0 | **Zero** – last ALU result was zero |
| N | 1 | **Negative** – result bit 15 is set (signed negative) |
| C | 2 | **Carry** – unsigned overflow (add) or borrow (sub) |
| V | 3 | **Overflow** – signed two's-complement overflow |

---

## Instruction Encoding

Every instruction is **exactly 4 bytes** (32 bits), stored little-endian.

### I-type (immediate operand)
```
 31      24  23   20  19   16  15               0
┌──────────┬────────┬────────┬──────────────────┐
│  opcode  │   rd   │  rs1   │     imm16         │
└──────────┴────────┴────────┴──────────────────┘
```

### R-type (register–register)
```
 31      24  23   20  19   16  15  12  11       0
┌──────────┬────────┬────────┬───────┬──────────┐
│  opcode  │   rd   │  rs1   │  rs2  │  (zero)  │
└──────────┴────────┴────────┴───────┴──────────┘
```
rs2 occupies bits [15:12] of the instruction word.

---

## Addressing Modes

| Mode | Syntax | Description |
|------|--------|-------------|
| Register | `ADD R0, R1, R2` | Operands are registers |
| Immediate | `LI R0, 42` or `ADDI R0, R1, -4` | 16-bit signed/unsigned constant |
| Register indirect + offset | `LOAD R0, [R1 + 4]` | Address = R1 + sign_ext(imm16) |
| Absolute (branch) | `JMP 0x0100` or `JMP label` | PC set to 16-bit target address |
| Register indirect (branch) | `JMPR R0` | PC set to register value |
| PC-relative | — | Not supported; use absolute labels |

---

## Instruction Reference

### Miscellaneous

| Mnemonic | Encoding | Operation | Flags |
|----------|----------|-----------|-------|
| `NOP` | opcode=0x00 | No operation | — |
| `HLT` | opcode=0x01 | Halt the CPU | — |

### R-type ALU  (rd = rs1 OP rs2)

| Mnemonic | Opcode | Operation | Flags |
|----------|--------|-----------|-------|
| `ADD rd, rs1, rs2` | 0x02 | rd = rs1 + rs2 | Z N C V |
| `SUB rd, rs1, rs2` | 0x03 | rd = rs1 – rs2 | Z N C V |
| `AND rd, rs1, rs2` | 0x04 | rd = rs1 & rs2 | Z N |
| `OR  rd, rs1, rs2` | 0x05 | rd = rs1 \| rs2 | Z N |
| `XOR rd, rs1, rs2` | 0x06 | rd = rs1 ^ rs2 | Z N |
| `NOT rd, rs1` | 0x07 | rd = ~rs1 | Z N |
| `SHL rd, rs1, rs2` | 0x08 | rd = rs1 << rs2 (logical) | Z N C |
| `SHR rd, rs1, rs2` | 0x09 | rd = rs1 >> rs2 (logical) | Z N C |
| `MUL rd, rs1, rs2` | 0x0A | rd = rs1 × rs2 (low 16 bits) | Z N C |
| `DIV rd, rs1, rs2` | 0x0B | rd = rs1 ÷ rs2 (unsigned) | Z N |
| `MOD rd, rs1, rs2` | 0x0C | rd = rs1 mod rs2 (unsigned) | Z N |
| `CMP rs1, rs2` | 0x0D | set flags(rs1 – rs2); rd unused | Z N C V |
| `MOV rd, rs1` | 0x0E | rd = rs1 | Z N |

### I-type ALU  (rd = rs1 OP sign_ext(imm16))

| Mnemonic | Opcode | Operation | Flags |
|----------|--------|-----------|-------|
| `ADDI rd, rs1, imm` | 0x10 | rd = rs1 + imm | Z N C V |
| `SUBI rd, rs1, imm` | 0x11 | rd = rs1 – imm | Z N C V |
| `ANDI rd, rs1, imm` | 0x12 | rd = rs1 & imm | Z N |
| `ORI  rd, rs1, imm` | 0x13 | rd = rs1 \| imm | Z N |
| `XORI rd, rs1, imm` | 0x14 | rd = rs1 ^ imm | Z N |
| `LI   rd, imm` | 0x15 | rd = imm (16-bit bit pattern) | Z N |
| `CMPI rs1, imm` | 0x16 | set flags(rs1 – imm); rd unused | Z N C V |

### Memory Access

Effective address = `rs1 + sign_ext(imm16)`

| Mnemonic | Opcode | Operation |
|----------|--------|-----------|
| `LOAD  rd, [rs1 + off]` | 0x20 | rd = mem16[addr] |
| `STORE rs, [rs1 + off]` | 0x21 | mem16[addr] = rs |
| `LOADB rd, [rs1 + off]` | 0x22 | rd = zero_ext(mem8[addr]) |
| `STOREB rs, [rs1 + off]`| 0x23 | mem8[addr] = rs[7:0] |

*For STORE/STOREB the "rd" field of the encoding carries the source register.*

### Control Flow

| Mnemonic | Opcode | Operation | Condition |
|----------|--------|-----------|-----------|
| `JMP  addr/label` | 0x30 | PC = imm16 | always |
| `JMPR rs1` | 0x31 | PC = rs1 | always |
| `JZ   addr` | 0x32 | PC = imm16 | if Z=1 |
| `JNZ  addr` | 0x33 | PC = imm16 | if Z=0 |
| `JN   addr` | 0x34 | PC = imm16 | if N=1 |
| `JNN  addr` | 0x35 | PC = imm16 | if N=0 |
| `JC   addr` | 0x36 | PC = imm16 | if C=1 |
| `JV   addr` | 0x37 | PC = imm16 | if V=1 |
| `CALL addr` | 0x38 | LR = PC+4; PC = imm16 | always |
| `CALLR rs1` | 0x39 | LR = PC+4; PC = rs1 | always |
| `RET` | 0x3A | PC = LR | always |

### Stack

The stack grows downward. SP points to the top of used stack.

| Mnemonic | Opcode | Operation |
|----------|--------|-----------|
| `PUSH rs` | 0x40 | SP -= 2; mem16[SP] = rs |
| `POP  rd` | 0x41 | rd = mem16[SP]; SP += 2 |

---

## Assembler Directives

| Directive | Example | Effect |
|-----------|---------|--------|
| `.org addr` | `.org 0x0100` | Set current address |
| `.word val` | `.word 0xABCD` | Emit 16-bit little-endian word |
| `.byte val` | `.byte 0xFF` | Emit one byte |
| `.string "text"` | `.string "hi\n"` | Emit null-terminated string (escape sequences: `\n \t \r \0 \\ \'`) |
| `.align n` | `.align 4` | Pad with zeros to reach next multiple of n |

---

## Numeric Literal Formats

| Format | Example | Value |
|--------|---------|-------|
| Decimal | `42`, `-10` | Signed decimal |
| Hexadecimal | `0xFF`, `0xF000` | Prefixed with `0x` |
| Binary | `0b1010` | Prefixed with `0b` |
| Character | `'A'`, `'\n'` | ASCII value |

---

## Memory Map Summary

| Range | Description |
|-------|-------------|
| `0x0000 – 0xEFFF` | RAM – 61 440 bytes |
| `0xEFFE` | Initial SP (stack grows downward into RAM) |
| `0xF000` | MMIO: STDOUT_CHAR (write → print ASCII char) |
| `0xF001` | MMIO: STDIN (read → get one char from terminal) |
| `0xF002` | MMIO: STDOUT_NUM (write → print decimal number + newline) |
| `0xF004` | MMIO: TIMER_LO (read → cycle count bits [15:0]) |
| `0xF006` | MMIO: TIMER_HI (read → cycle count bits [31:16]) |
| `0xF008 – 0xFFFF` | Reserved |
