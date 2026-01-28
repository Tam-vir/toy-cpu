;fill the screen with a color

LDI R5, 0
LDI R6, 0
LDI R7, 0x40
LUI R7, 0x01
LDI R9, 200
LDI R3, 4        ; R3 = color (7 = pink... see vga.cpp for all palettes)

y_loop:
MOV R6, R0

loop:
MOV R2, R5
MUL R2, R7
ADD R2, R6
ST R2, 0xFF01    ; VRAM addr low byte
ST R2, 0xFF02    ; VRAM addr high byte
ST R3, 0xFF03    ; VRAM data write
INC R6
CMP R6, R7
JNZ loop

INC R5
CMP R5, R9
JNZ y_loop

ST R2, 0xFF06    ; Trigger VGA present

HALT