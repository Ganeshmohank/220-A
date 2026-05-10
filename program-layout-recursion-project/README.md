# Program Layout and Recursion Project

This submission demonstrates a **recursive factorial** in C, how a simple **executable is laid out in memory**, and how **function calls and recursion** use the **stack**—in terms that align with a **Software CPU Design** (program counter, stack pointer, frame pointer, code vs. data vs. stack).

## What this project shows

- A small **driver** (`main.c`) calls `factorial(5)`.
- **Trace messages** print when each recursive call **enters** and **returns**, so you can see the call chain and unwind.
- Documentation under `memory-layout/` explains **text, data, heap, stack**, and **recursive stack frames** for `factorial(5)`.
- A **report** and **video script** support the coursework write-up and recording.

## Folder structure

| Path | Purpose |
|------|---------|
| `src/main.c` | Driver: calls `factorial(5)`, prints summary |
| `src/factorial.c` | Recursive `factorial` with enter/return traces |
| `include/factorial.h` | Declaration of `factorial` |
| `Makefile` | Build `bin/recursion_demo`; targets `run`, `clean` |
| `memory-layout/` | Executable layout, stack frames, sample trace |
| `report/` | Report (Markdown) and double-spacing template notes |
| `video/` | Video script and demo recording plan |
| `docs/` | Short rubric-oriented project summary |

## Prerequisites

- **gcc** and **make** (typical on Linux/macOS; Windows users can use WSL or MSYS2 with the same commands).

## Download, compile, and run

Replace the URL with your real team repository when you publish.

```bash
git clone [Insert GitHub Repository Link Here]
cd program-layout-recursion-project
make
make run
make clean
```

### Expected output for `factorial(5)`

After `make run`, you should see trace lines for calls `factorial(5)` down to `factorial(1)`, then returns unwinding until the final result **120** is printed in `main`.

See `memory-layout/sample_trace_factorial_5.txt` for a full sample that matches the program.

## How this relates to the CPU design course

The program is ordinary C, but the docs describe execution as a **CPU** would see it: **instructions in low memory (text)**, **stack growth toward higher addresses** (conceptual diagram), **PC** stepping through instructions, **SP** tracking the stack top, and **FP** (frame pointer) helping access locals and **return addresses**—the same ideas you used when reasoning about a software CPU.

## Clean build artifacts

```bash
make clean
```

Removes `bin/` and `build/`.
