# Video Script: Function Calls, Stack Frames, and Recursion (~3–5 minutes)

**Speaker notes:** Speak clearly; pause after running commands so viewers can read the terminal.

---

### [0:00–0:30] Opening — project goal

“Hi, this is our coursework demo for **program layout and recursion**. We built a small C program that computes **factorial of five** using **recursion**. The goal is to connect normal C code to how a **CPU** runs it: **instructions in memory**, a **program counter**, and a **stack** of **function frames**—the same big ideas from our **Software CPU Design** project.”

---

### [0:30–1:00] What the program does

“We implement **factorial(n)** recursively: the **base case** is when **n** is **0 or 1**, we return **1**. Otherwise we return **n times factorial of n minus one**. Our **main** function calls **factorial(5)** and prints the answer, which should be **one hundred twenty**.”

---

### [1:00–1:45] Compile and run

“On a Linux or Mac terminal—or WSL on Windows—we use **gcc** and **make**. From the folder **program-layout-recursion-project**, we run **make**, then **make run**. **Make** creates a **bin** directory and places the executable **recursion_demo** there. **Make run** executes it.”

*(Demo: type `make` and `make run`.)*

“You should see lines showing each call **entering** factorial with smaller and smaller **n**, then **returning** after the base case, until **main** prints the final result.”

---

### [1:45–2:45] Factorial recursion and how **main** calls **factorial**

“When **main** calls **factorial(5)**, the compiler generates a normal **call**: set up the argument **five**, remember the **return address** back in **main**, and jump into **factorial**. Inside **factorial(5)**, since five is greater than one, we need **factorial(4)**—so we **call again**. That repeats until **factorial(1)** hits the **base case** and returns **one** without calling deeper.”

“So recursion is not a different CPU instruction—it is the **same call mechanism**, just calling the **same function** again before the previous call finishes.”

---

### [2:45–3:45] Memory: text, stack, PC, SP, FP

“Think of the **executable** in memory: the **text segment** holds **machine instructions** for **main** and **factorial**. The **stack** holds **frames**—one per **active** call. The **program counter** points at the **next instruction** in the text segment. The **stack pointer** tracks the **top** of the stack as we **push** information for each call.”

“Many systems also use a **frame pointer** so each function can find its **locals** and the **saved return context** at predictable offsets. Each **factorial** frame stores things like the parameter **n** and temporary values used when we multiply after the inner call returns.”

---

### [3:45–4:30] Pushing and popping frames; how **120** appears

“As we go from **five** down to **one**, we conceptually **push** deeper frames. At the bottom, **factorial(1)** returns **one**. Then each caller **pops** back to life at its **return address**, multiplies, and returns: **two** times one is **two**, **three** times two is **six**, **four** times six is **twenty-four**, **five** times twenty-four is **one hundred twenty**. That value travels back to **main**, which prints it.”

---

### [4:30–5:00] Tie back to Software CPU Design + closing

“This mirrors our **software CPU** mental model: fetch-decode-execute with a **PC**, memory for **code**, and a **stack** for **calls**. Recursion simply **reuses** the call machinery. Thanks for watching—see our repo’s **memory-layout** docs for diagrams and a full trace of **factorial five**.”

---

**Total target length:** about **3–5 minutes** at a comfortable pace; trim pauses or the opening if you need to stay under a strict limit.
