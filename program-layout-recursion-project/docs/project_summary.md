# Project Summary (Rubric Alignment)

## Overview

The **`program-layout-recursion-project`** folder contains a **recursive factorial** in C, a **Makefile** build, documentation of **executable memory layout** and **stack frames**, a **report** outline, and a **video script**—all scoped to this submission folder without changing the existing CPU project at the repository root.

## How this satisfies the rubric

### It works

- Running **`make`** then **`make run`** builds **`bin/recursion_demo`** and prints a full **trace** for **`factorial(5)`** ending in **120**.

### Implementation: commented and formatted

- **`factorial.c`** documents base vs. recursive case; **`main.c`** documents the driver role.
- **`-Wall -Wextra -std=c99 -pedantic`** encourages clean, portable C.

### Organization and presentation

- Clear directory tree: **source**, **headers**, **memory-layout**, **report**, **video**, **docs**.
- **`README.md`** gives exact **clone / make / run** commands and points to the trace file.

### Team contributions

- **`report/report.md`** includes a **contributions table** with placeholders for each member to fill before final PDF/Word submission.

## Relation to the Software CPU Design project

Concepts (**text segment**, **stack**, **PC**, **SP**, **FP**, **return address**) are explained in **`memory-layout/`** so this submission **extends** the earlier CPU-design coursework without modifying those original files.
