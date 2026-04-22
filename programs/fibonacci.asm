; =============================================================================
;  fibonacci.asm  –  Compute and print the first 10 Fibonacci numbers
; =============================================================================
;
;  The Fibonacci sequence: F(0)=0, F(1)=1, F(n) = F(n-1) + F(n-2)
;
;  Expected output (one value per line):
;    0
;    1
;    1
;    2
;    3
;    5
;    8
;    13
;    21
;    34
;
;  Register allocation:
;    R0  –  F(n-2)  (previous-previous Fibonacci value)
;    R1  –  F(n-1)  (previous Fibonacci value)
;    R2  –  F(n)    (current Fibonacci value / scratch)
;    R3  –  total count limit (10)
;    R4  –  loop index  i
;    R5  –  STDOUT_NUM MMIO address (0xF002)
;
;  Algorithm trace:
;    i=0  → print R0 (0)
;    i=1  → print R1 (1)
;    i=2  → R2=R0+R1=1,  print 1;  R0←R1=1,   R1←R2=1
;    i=3  → R2=R0+R1=2,  print 2;  R0←1,      R1←2
;    i=4  → R2=1+2=3,    print 3;  R0←2,      R1←3
;    i=5  → R2=2+3=5,    print 5;  R0←3,      R1←5
;    i=6  → R2=3+5=8,    print 8;  R0←5,      R1←8
;    i=7  → R2=5+8=13,   print 13; R0←8,      R1←13
;    i=8  → R2=8+13=21,  print 21; R0←13,     R1←21
;    i=9  → R2=13+21=34, print 34; (loop ends)
;
;  MMIO:
;    0xF002  STDOUT_NUM  – write: 16-bit value as signed decimal + newline
; =============================================================================

        .org 0x0000

        ; ── Initialise registers ────────────────────────────────────────────
        LI   R5, 0xF002     ; R5 = STDOUT_NUM MMIO address
        LI   R0, 0          ; R0 = F(0) = 0  (F_prev_prev)
        LI   R1, 1          ; R1 = F(1) = 1  (F_prev)
        LI   R3, 10         ; R3 = total numbers to print
        LI   R4, 0          ; R4 = loop index i = 0

        ; ── Print F(0) and F(1) directly ────────────────────────────────────
        STORE R0, [R5 + 0]  ; output 0
        STORE R1, [R5 + 0]  ; output 1
        ADDI  R4, R4, 2     ; i = 2  (already printed two values)

        ; ── Main loop: compute and print F(i) for i = 2 … 9 ───────────────
loop:
        CMP   R4, R3        ; compare i with count limit
        JZ    done          ; if i == 10, we are finished

        ; ── COMPUTE: F(n) = F(n-2) + F(n-1) ────────────────────────────────
        ADD   R2, R0, R1    ; R2 = F(n-2) + F(n-1)  [ALU ADD operation]

        ; ── STORE: send result to output ─────────────────────────────────────
        STORE R2, [R5 + 0]  ; write F(n) to STDOUT_NUM MMIO

        ; ── FETCH next iteration's operands: slide the window forward ────────
        MOV   R0, R1        ; F(n-2) ← old F(n-1)
        MOV   R1, R2        ; F(n-1) ← newly computed F(n)

        ADDI  R4, R4, 1     ; i++
        JMP   loop          ; repeat

done:
        HLT                 ; all 10 values printed – halt
