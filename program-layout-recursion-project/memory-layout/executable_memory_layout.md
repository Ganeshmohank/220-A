# Executable Memory Layout (Software CPU View)

This project uses a tiny C program with **no heap allocation** and **no non-trivial global data**. Even so, the operating system still loads an **executable image** into the process’s virtual address space. Below is a **simplified** layout tied to ideas from a **Software CPU Design**: where **code** lives, where the **stack** lives, and how **PC**, **SP**, and **FP** participate during `main` and `factorial`.

## Major regions

### Text (code) segment

- Holds **machine instructions** compiled from `main` and `factorial` (and startup code linked by the toolchain).
- Typically **read-only** and shared if multiple processes run the same program.
- The **Program Counter (PC)** points into this region (conceptually: “the address of the next instruction to execute”).

### Data segment

- Holds **initialized global and static** variables.
- This demo barely uses globals; still, the runtime may place small objects here (e.g. string literals may live in read-only data depending on platform).

### Heap

- Region for **dynamic allocation** (`malloc`, etc.).
- **Not used** in this simple factorial program.

### Stack

- Grows as functions **call** and **return**.
- Each **activation** of a function uses a **stack frame**: space for **return address**, **saved frame pointer**, **parameters**, **local variables**, and **temporary** values.
- The **Stack Pointer (SP)** indicates the **top** of the stack (exact direction—toward higher or lower addresses—is ABI-specific; diagrams often show stack toward **high** memory).
- The **Frame Pointer (FP)** (when used) points within the **current** frame so the code can reach locals and the **saved return context** with stable offsets.

## Conceptual diagram (high memory at top)

Many textbooks draw the stack toward **high** addresses and code/data toward **low** addresses. This is **schematic**, not a literal map of your OS memory.

```
High Memory
+----------------------+
| Stack                |
|   ...                |
|   factorial(1) frame |
|   factorial(2) frame |
|   factorial(3) frame |
|   factorial(4) frame |
|   factorial(5) frame |
|   main frame         |
+----------------------+
|        (gap)         |
+----------------------+
| Heap                 |
|   (unused here)      |
+----------------------+
| Data Segment         |
|   globals/statics    |
+----------------------+
| Text Segment         |
|   main instructions  |
|   factorial instr. |
+----------------------+
Low Memory
```

## CPU registers during a call

| Register / concept | Role |
|---------------------|------|
| **PC** | Address of next instruction; advances as instructions execute; updated on **call** (jump to callee) and **return** (jump back). |
| **SP** | Tracks stack top; changes as values are **pushed** and **popped**. |
| **FP** | Optional but common: anchor for addressing **locals** and accessing the **caller’s frame** via fixed offsets. |

When `main` calls `factorial(5)`, the compiler emits code to **pass `5`**, **save the return address**, and **transfer control**. Each deeper `factorial(n-1)` repeats that pattern until the **base case** returns; then returns **pop** frames conceptually and **PC** resumes in the caller.

## Relation to this repository’s C program

- **`main` and `factorial` instructions** → text segment.
- **Automatic variables** (e.g. `smaller`, `result` in `factorial`) → typically in the **current stack frame**.
- **Recursive depth** → **multiple frames** for `factorial` at once, one per unfinished call.

See `stack_frames_factorial.md` for the factorial(5) call chain in detail.
