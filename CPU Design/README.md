# SPARTAN-16 — Software CPU in C++

A complete 16-bit CPU emulator and assembler built in C++17.
Includes a custom ISA, ALU, control unit, memory-mapped I/O, two-pass assembler,
and three demo programs.

---

## Before you run (prerequisites)

You need a normal developer setup—not extra libraries, just a compiler and `make`.


| Requirement          | Notes                                                                                                                     |
| -------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| **Operating system** | macOS or Linux (Windows: use WSL2 or adapt the commands).                                                                 |
| `**make`**           | Usually pre-installed on macOS; on Linux: `sudo apt install build-essential` (Debian/Ubuntu) or your distro’s equivalent. |
| `**g++` with C++17** | The `Makefile` uses `g++`. Check: `g++ --version`. On macOS, install Xcode Command Line Tools: `xcode-select --install`.  |


**Download the project** (zip from your host/GitHub, or `git clone`). If the folder name has spaces (e.g. `220 A project`), quote paths in the terminal:

```bash
cd "/path/to/220 A project"
```

---

## Quick start (easiest path for graders / recordings)

One script builds everything and runs Hello World:

```bash
chmod +x demo.sh    # first time only (Unix)
./demo.sh           # default: hello demo
./demo.sh fibonacci
./demo.sh timer
```

Or manually: `make` then `./cpu exec programs/hello.asm` (see **Demo Commands** below).

---

## Project Structure

```
220-A/
└── CPU Design/
    ├── src/
    │   ├── common.h          Shared type aliases and memory-map constants
    │   ├── isa.h / isa.cpp   ISA: opcodes, encoding, decoding, disassembler
    │   ├── memory.h/.cpp     Physical RAM (64 KB byte array)
    │   ├── cpu.h/.cpp        CPU state: register file, flags, PC, cycle counter
    │   ├── alu.h/.cpp        ALU: arithmetic, logic, shifts, flag computation
    │   ├── bus.h/.cpp        System bus: routes reads/writes to RAM or MMIO
    │   ├── control_unit.h/.cpp  Control unit: Fetch / Decode / Execute pipeline
    │   ├── emulator.h/.cpp   Top-level: load binary, run, hex-dump memory
    │   ├── assembler.h/.cpp  Two-pass assembler: labels, directives, opcodes
    │   └── main.cpp          CLI entry point (assemble / run / exec commands)
    ├── programs/
    │   ├── timer.asm         Timer demo — illustrates Fetch/Compute/Store cycles
    │   ├── hello.asm         Hello, World! — string output via MMIO
    │   └── fibonacci.asm     Fibonacci sequence — first 10 values
    ├── docs/
    │   ├── schematic.txt     ASCII-art CPU architecture diagram
    │   └── isa.md            Full ISA reference (registers, opcodes, addressing)
    ├── Makefile
    └── README.md
```

---

## Build

From the `CPU Design` directory (where the `Makefile` lives):

```bash
cd "CPU Design"
make
```

This compiles all `src/*.cpp` files and produces the `./cpu` executable. The same step runs automatically if you use `./demo.sh`.

---

## Demo Commands

Run these **in order** to see every feature of SPARTAN-16 (after `make` or `./demo.sh` has built `cpu` once).

### Step 1 — Hello, World!

```bash
./cpu exec programs/hello.asm
```

Expected output:

```
Assembled 51 bytes → 'programs/hello.bin'  (load addr 0x0000)
Loaded 51 bytes at 0x0000 from 'programs/hello.bin'

──────────────────────────────────────────────
  SPARTAN-16  Running: programs/hello.bin
──────────────────────────────────────────────
Hello, World!

Halted after 90 instruction(s).
```

### Step 2 — Hello, World! with full trace

```bash
./cpu exec programs/hello.asm --trace
```

This prints the **Fetch / Decode / Execute** log for every instruction,
showing PC, disassembled opcode, flags, and cycle count.

### Step 3 — Fibonacci Sequence

```bash
./cpu exec programs/fibonacci.asm
```

Expected output (first 10 Fibonacci numbers):

```
0
1
1
2
3
5
8
13
21
34

Halted after 75 instruction(s).
```

### Step 4 — Fibonacci with trace

```bash
./cpu exec programs/fibonacci.asm --trace
```

### Step 5 — Timer demo (Fetch/Compute/Store)

```bash
./cpu exec programs/timer.asm
```

Counts 1–10, then prints the number of CPU cycles the loop took.

### Step 6 — Timer with trace

```bash
./cpu exec programs/timer.asm --trace
```

### Step 7 — Assemble only (produce a .bin file)

```bash
./cpu assemble programs/fibonacci.asm -o fibonacci.bin
```

### Step 8 — Run a pre-assembled binary

```bash
./cpu run fibonacci.bin
```

### Step 9 — Memory dump after execution

```bash
./cpu exec programs/hello.asm --dump
```

Prints a hex + ASCII dump of the first 256 bytes of RAM, showing the
assembled instructions and the string data in memory.

### Step 10 — Makefile convenience targets

```bash
make hello           # exec hello.asm
make fibonacci       # exec fibonacci.asm
make timer           # exec timer.asm
make trace-fibonacci # exec fibonacci.asm --trace
make dump-hello      # exec hello.asm --dump
```

---

## Clean

```bash
make clean
```

Removes compiled objects, the `cpu` binary, and any `.bin` files.

---

## ISA Quick Reference

See `docs/isa.md` for the full instruction set reference.
See `docs/schematic.txt` for the ASCII architecture diagram.

Key facts:

- **35 opcodes**: ALU (R-type + I-type), memory, branches, stack
- **4-byte fixed-width instructions** (little-endian)
- **MMIO at 0xF000–0xFFFF**: character output, number output, timer, stdin
- **Stack** grows downward from 0xEFFE
- **Two-pass assembler**: supports labels, `.org`, `.string`, `.word`, `.byte`, `.align`

