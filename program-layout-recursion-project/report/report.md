# Program Layout and Recursion on a Software CPU Model

**Course submission report (Markdown source)**  
Convert to your instructor’s required format (PDF/Word) using `report_template_double_spaced.md` for margin, spacing, and page layout notes.

---

## Title page

**Project title:** Program Layout, Function Calls, and Recursion (Factorial Demo)

**Course / institution:** [Course name and number]

**Date:** [Submission date]

**Team members:**

- [Team Member 1 Name]
- [Team Member 2 Name]
- [Team Member 3 Name]

---

## GitHub repository

**Repository URL:**  
[Insert GitHub Repository Link Here]

This link should point to the root of the course repository that contains both the earlier Software CPU Design project and the folder `program-layout-recursion-project/`.

---

## Download, compile, and run

**Prerequisites:** `gcc`, `make` (Linux or macOS terminal; or WSL on Windows).

```bash
git clone [Insert GitHub Repository Link Here]
cd program-layout-recursion-project
make
make run
make clean
```

- **`make`** creates `bin/` if needed, compiles `src/main.c` and `src/factorial.c`, and writes `bin/recursion_demo`.
- **`make run`** executes `./bin/recursion_demo`.
- **`make clean`** removes `bin/` and `build/`.

**Expected result:** trace output for `factorial(5)` ending with `120`. A saved sample is in `memory-layout/sample_trace_factorial_5.txt`.

---

## Team member contributions

| Team member | Contribution |
|-------------|--------------|
| [Team Member 1 Name] | [Describe: e.g. C implementation, tracing, testing on Linux] |
| [Team Member 2 Name] | [Describe: e.g. memory layout documentation, diagrams] |
| [Team Member 3 Name] | [Describe: e.g. report, video script, README polish] |

*Replace bracketed text with accurate, specific descriptions before submission.*

---

## Project explanation

We implemented a **recursive factorial** in C and a **driver** in `main` that computes **5!**. The factorial function prints **enter** and **return** messages so readers can see **which call is active** and **how control returns** after the base case.

This satisfies the assignment goal: show a **recursive function**, a **main** program that calls it, and explain how the **executable conceptually occupies memory** and how **calls and recursion** use the **stack**, consistent with a **Software CPU Design** mindset (PC, SP, FP, segments).

---

## Memory layout explanation (summary)

A process image can be thought of in **segments**:

- **Text:** machine instructions for `main`, `factorial`, and runtime glue.
- **Data:** initialized globals/statics (minimal here).
- **Heap:** unused in this program (no `malloc`).
- **Stack:** one **frame** per active function call; recursion means **many** frames for the **same** function at different depths.

Details and a diagram are in `memory-layout/executable_memory_layout.md`.

---

## Function calls and recursion (summary)

A **function call** saves a **return address**, sets up **parameters**, and transfers control. **Recursion** repeats that process with the **same** function: each invocation has its **own** `n` and **own** locals on the **stack**. The **base case** stops further calls; **returns** then **unwind** the stack, multiplying partial results until `factorial(5)` yields **120**.

Step-by-step for `factorial(5)` is in `memory-layout/stack_frames_factorial.md`.

---

## Files submitted (reference)

- **Source:** `src/main.c`, `src/factorial.c`, `include/factorial.h`
- **Build:** `Makefile`, output `bin/recursion_demo`
- **Docs:** `README.md`, `memory-layout/*`, `video/*`, `docs/project_summary.md`
