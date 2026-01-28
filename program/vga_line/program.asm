LDI R5, 0
LDI R6, 100
LDI R7, 0x40
LUI R7, 0x01

LDI R3, 2

loop:
MOV R2, R6
MUL R2, R7
ADD R2, R5

ST R2, 0xFF01    ; VRAM addr low byte
ST R2, 0xFF02    ; VRAM addr high byte

ST R3, 0xFF03    ; VRAM data write

INC R5
CMP R5, R7
JNZ loop

ST R3, 0xFF06

HALT