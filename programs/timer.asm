; =============================================================================
;  timer.asm  –  Timer demonstration showing Fetch / Compute / Store cycles
; =============================================================================
;
;  This program counts from 1 to 10, printing each value to the numeric
;  STDOUT MMIO register.  After the loop it reads the hardware timer and
;  reports how many CPU cycles the loop consumed.
;
;  Every iteration of the loop is a textbook Fetch / Compute / Store cycle:
;
;    FETCH   –  The counter value is already in register R0 (loaded or
;               incremented in the previous cycle).
;    COMPUTE –  ADDI adds 1  (ALU operation).
;    STORE   –  STORE writes the result to the MMIO output register.
;
;  MMIO memory map (see common.h):
;    0xF002  STDOUT_NUM  – write: print 16-bit value as decimal + newline
;    0xF004  TIMER_LO    – read:  CPU cycle counter bits [15:0]
; =============================================================================

        .org 0x0000

        ; ── Initialise registers ────────────────────────────────────────────
        LI   R5, 0xF002     ; R5 = STDOUT_NUM  (decimal output MMIO)
        LI   R6, 0xF004     ; R6 = TIMER_LO    (cycle counter MMIO)
        LI   R1, 10         ; R1 = loop limit (count up to 10)
        LI   R0, 0          ; R0 = counter     (starts at 0)

        ; ── Snapshot the timer before the loop ─────────────────────────────
        LOAD R3, [R6 + 0]   ; R3 = starting cycle count

        ; ── Main loop ───────────────────────────────────────────────────────
        ;   Each iteration demonstrates one complete F/C/S pipeline cycle.
loop:
        ; ── FETCH   ─────────────────────────────────────────────────────────
        ;   The current counter value is already in R0 (register access
        ;   IS the fetch for a register-resident value).

        ; ── COMPUTE ─────────────────────────────────────────────────────────
        ADDI R0, R0, 1      ; counter++ (ALU add-immediate operation)

        ; ── STORE   ─────────────────────────────────────────────────────────
        STORE R0, [R5 + 0]  ; write counter to STDOUT_NUM MMIO → prints value

        ; ── Loop control ─────────────────────────────────────────────────────
        CMP  R0, R1         ; compare counter with limit
        JNZ  loop           ; if counter != limit, continue looping

        ; ── Report elapsed cycles ────────────────────────────────────────────
        LOAD  R4, [R6 + 0]  ; R4 = ending cycle count
        SUB   R2, R4, R3    ; R2 = elapsed = end - start
        STORE R2, [R5 + 0]  ; print elapsed cycle count

done:
        HLT                 ; stop execution
