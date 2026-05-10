# Stack Frames for `factorial(5)`

This document explains how **recursive** calls to `factorial` build and tear down **stack frames** for the default demo `factorial(5)`.

## Call chain (downward)

1. **`main`** calls **`factorial(5)`**.
2. **`factorial(5)`** calls **`factorial(4)`** (because `5 > 1`).
3. **`factorial(4)`** calls **`factorial(3)`**.
4. **`factorial(3)`** calls **`factorial(2)`**.
5. **`factorial(2)`** calls **`factorial(1)`**.
6. **`factorial(1)`** hits the **base case** (`n <= 1`) and **returns `1`** without calling deeper.

## Unwind (upward)

After the base case, each caller multiplies by `n` and returns:

- **`factorial(1)`** returned **1**.
- **`factorial(2)`** returns **2 × 1 = 2**.
- **`factorial(3)`** returns **3 × 2 = 6**.
- **`factorial(4)`** returns **4 × 6 = 24**.
- **`factorial(5)`** returns **5 × 24 = 120**.
- **`main`** receives **120**.

## Return addresses

Each **call** does roughly:

1. Place **arguments** (here, `n`) where the ABI expects them (registers and/or stack).
2. Record **where to resume** after the callee returns—the **return address** (often pushed implicitly by a `call`-like instruction or written into the callee’s frame).
3. Jump to the **start of `factorial`**.

When **`factorial(k)`** calls **`factorial(k-1)`**, the frame for **`factorial(k)`** stays **active** but **waiting**: its **return address** points to the instruction **after** the inner call (where it will use the returned value and multiply).

**Recursion** is not special hardware magic: it is the **same** call mechanism as a normal function call; you simply happen to call **the same function** again before the previous instance has finished.

## Call stack diagram (deepest at top)

Below, “top” is the **most recent** call (closest to current SP in a typical diagram):

```
                    +------------------+
                    | factorial(1)     |  base case: returns 1
                    +------------------+
                    | factorial(2)     |  waits for factorial(1)
                    +------------------+
                    | factorial(3)     |  waits for factorial(2)
                    +------------------+
                    | factorial(4)     |  waits for factorial(3)
                    +------------------+
                    | factorial(5)     |  waits for factorial(4)
                    +------------------+
                    | main             |  waits for factorial(5)
                    +------------------+
```

Each box is a **stack frame** holding (at least conceptually):

- **Return address** (where to go back when this call finishes).
- **Saved caller FP** (if the ABI uses a frame pointer chain).
- **`n`** (the parameter for this activation).
- **Locals** such as **`smaller`** and **`result`** (in our `factorial.c`), allocated in **this** frame.

When **`factorial(1)`** returns, its frame is **reclaimed**; control resumes in **`factorial(2)`** at the **return address** after the call to **`factorial(1)`**, with the return value available (conceptually in a register or stack slot per ABI).

## Tie-in to the Software CPU Design

Your earlier CPU model likely had a **PC**, **stack** or equivalent, and rules for **call** and **return**. Recursion here is the same idea: each **nested** call is a **new** frame; the **CPU** always executes **one** instruction stream, but the **data** on the stack remembers **all** suspended calls until they complete.
