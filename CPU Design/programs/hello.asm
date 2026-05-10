; =============================================================================
;  hello.asm  –  Print "Hello, World!" followed by a newline
; =============================================================================
;
;  Demonstrates:
;    • LOADB  – byte-level memory read (string traversal)
;    • CMPI   – compare register with immediate (null-terminator check)
;    • STORE  – MMIO character output
;    • Labels & label-based addressing (str, print_loop, done)
;    • Null-terminated .string directive
;
;  MMIO memory map:
;    0xF000  STDOUT_CHAR  – write: lower byte printed as ASCII character
;
;  Memory layout after assembly (load address 0x0000):
;    0x0000  LI   R5, 0xF000       (instruction)
;    0x0004  LI   R6, str          (instruction – str resolved to 0x0024)
;    0x0008  LOADB R0, [R6]        (instruction)  ← print_loop
;    0x000C  CMPI  R0, 0           (instruction)
;    0x0010  JZ    done            (instruction)
;    0x0014  STORE R0, [R5]        (instruction)
;    0x0018  ADDI  R6, R6, 1       (instruction)
;    0x001C  JMP   print_loop      (instruction)
;    0x0020  HLT                   (instruction)  ← done
;    0x0024  "Hello, World!\n\0"   (data, 15 bytes) ← str
; =============================================================================

        .org 0x0000

        ; ── Initialise ──────────────────────────────────────────────────────
        LI   R5, 0xF000     ; R5 = STDOUT_CHAR MMIO address
        LI   R6, str        ; R6 = pointer to start of string data

        ; ── Print loop ──────────────────────────────────────────────────────
print_loop:
        LOADB R0, [R6 + 0]  ; R0 = byte at current pointer (one character)
        CMPI  R0, 0         ; is this the null terminator?
        JZ    done          ; yes → stop printing
        STORE R0, [R5 + 0]  ; write character to STDOUT_CHAR MMIO
        ADDI  R6, R6, 1     ; advance string pointer by one byte
        JMP   print_loop    ; fetch next character

done:
        HLT

        ; ── String data (placed after all instructions) ──────────────────────
str:
        .string "Hello, World!\n"
