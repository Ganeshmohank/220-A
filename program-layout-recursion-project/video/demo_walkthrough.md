# Demo Walkthrough: Step-by-Step Recording Plan

Use this checklist while recording the video (see `video_script.md` for spoken content).

## Before recording

1. Clone or open the repository locally; ensure **terminal** font size is readable on video.
2. Terminal at repo root; know the path to **`program-layout-recursion-project`**.

## Recording steps

1. **Show GitHub repo**  
   - Browser: open the team repository page.  
   - Scroll briefly to show it is a real project (no passwords on screen).

2. **Show folder structure**  
   - In file manager or IDE sidebar: open **`program-layout-recursion-project`**.  
   - Point out **`src/`**, **`include/`**, **`memory-layout/`**, **`report/`**, **`video/`**, **`docs/`**.

3. **Open source code**  
   - Open **`src/main.c`**: note **`factorial(5)`** and banner prints.  
   - Open **`src/factorial.c`**: point to **base case** (`n <= 1`) and **recursive case**.

4. **Build**  
   - Terminal:  
     `cd program-layout-recursion-project`  
     `make`  
   - Confirm **`bin/recursion_demo`** (or show **`ls bin`** / **`dir bin`** on Windows).

5. **Run**  
   - `make run`  
   - Let output scroll; pause on final **120**.

6. **Explain output**  
   - Trace **`ENTER`** from 5 down to 1.  
   - Trace **`RETURN`** from 1 up to 120.

7. **Show memory layout docs**  
   - Open **`memory-layout/executable_memory_layout.md`**: segments diagram.  
   - Open **`memory-layout/stack_frames_factorial.md`**: call stack diagram.

8. **Summarize recursion**  
   - Same as normal calls; multiple **frames**; **PC** and **return addresses**; result **unwinds**.

## Optional B-roll

- Show **`memory-layout/sample_trace_factorial_5.txt`** side by side with terminal output to show they match.

## Done

- Stop recording; save with a clear filename (e.g. `recursion_demo_teamX.mp4`).
