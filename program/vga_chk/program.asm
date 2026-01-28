; Checkerboard 320x200, 40x40 blocks
; Colors: green=2, white=5

LDI R4, 0x40      ; screen width low
LUI R4, 0x01      ; screen width high -> 320
LDI R5, 200       ; screen height = 200
LDI R6, 40        ; block size
LDI R7, 0         ; y counter
LDI R11, 255      ; (not used in this code)
LDI R12, 1        ; for CMP
LDI R13, 2        ; for MOD
LDI R14, 0        ; x counter (replaces R15)
LDI R15, 0        ; available for temp calculations

row_loop:
    MOV R14, R0    ; reset x counter (R15 is 0)

col_loop:
    ; Compute VRAM address = y*width + x
    ; Use R15 as temp register for calculations
    MOV R15, R7
    MUL R15, R4      ; R15 = y * width
    ADD R15, R14     ; R15 = y*width + x
    MOV R9, R15      ; save VRAM address in R9

    ; Compute block indices
    MOV R1, R7
    DIV R1, R6       ; y/block -> R1 = y_block
    MOV R2, R14
    DIV R2, R6       ; x/block -> R2 = x_block
    ADD R1, R2       ; R1 = x_block + y_block
    MOV R2, R13      ; R2 = 2
    MOD R1, R2       ; R1 = (x_block + y_block) % 2

    LDI R3, 2        ; default green
    CMP R1, R12      ; compare with 1
    JZ set_white
    JMP draw_pixel

set_white:
    LDI R3, 5        ; white color

draw_pixel:
    ; Write VRAM address (low/high)
    ; VRAM address low byte
    MOV R15, R9      ; get VRAM address from R9
    ST R15, 0xFF01   ; write low byte
    
    ; VRAM address high byte
    MOV R15, R9
    ST R15, 0xFF02   ; write high byte

    ; Write color and draw
    ST R3, 0xFF03

    ; Increment x
    INC R14
    CMP R14, R4
    JNZ col_loop

    ; Increment y
    INC R7
    CMP R7, R5
    JNZ row_loop

; Trigger VGA present
LDI R1, 1
ST R1, 0xFF06

HALT