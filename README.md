# SPARTAN-16 — Software CPU in C++

A complete 16-bit CPU emulator and assembler built in C++17.
Includes a custom ISA, ALU, control unit, memory-mapped I/O, two-pass assembler,
and three demo programs.

---

## Before you run (prerequisites)

You need a normal developer setup—not extra libraries, just a compiler and `make`.

| Requirement | Notes |
|-------------|--------|
| **Operating system** | macOS or Linux (Windows: use WSL2 or adapt the commands). |
| **`make`** | Usually pre-installed on macOS; on Linux: `sudo apt install build-essential` (Debian/Ubuntu) or your distro’s equivalent. |
| **`g++` with C++17** | The `Makefile` uses `g++`. Check: `g++ --version`. On macOS, install Xcode Command Line Tools: `xcode-select --install`. |

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

## Recording / what to say (suggested order)

Use this as a loose script while you screen-record; adjust timing to your rubric.

1. **Intro (your project in one sentence)**  
   “This is SPARTAN-16—a 16-bit CPU simulated in software in C++17, with its own instruction set, assembler, and demos that run on the emulator.”

2. **What someone needs on their machine**  
   “To run it you only need `make` and `g++` with C++17—no extra packages. On a Mac that’s usually the Xcode Command Line Tools.”

3. **High-level architecture (before live commands)**  
   “The design has a register file, ALU, control unit doing fetch–decode–execute, memory, and a bus that maps part of the address space to I/O. The diagram is in `docs/schematic.txt`, and the full ISA is in `docs/isa.md`.”  
   *(Optional: show `docs/schematic.txt` or your block diagram for 30–60 seconds.)*

4. **Prove it’s easy to run**  
   “From the project folder I’ll run the provided script—it builds and runs the hello program.”  
   Run: `./demo.sh` (or `make && ./cpu exec programs/hello.asm`).

5. **Show one more behavior**  
   “Here’s Fibonacci output” / “here’s execution with `--trace` so you can see each instruction” — pick one short command from **Demo Commands**.

6. **Close**  
   “Source is under `src/`, assembly demos under `programs/`, and the README lists all commands for assemble-only and pre-built binaries.”

**File-by-file tour + Fibonacci-only narration:** see [`docs/demo-video-script.md`](docs/demo-video-script.md) (what to say for each source file and a line-by-line Fibonacci walkthrough for the demo video).

---

## Project Structure

```
220 A project/
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
├── demo.sh               One-command build + demo (hello / fibonacci / timer)
└── README.md
```

---

## Build

```bash
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
