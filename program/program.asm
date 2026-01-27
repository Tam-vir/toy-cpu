; Print "Hello" using MMIO
LDI R0, 0        ; offset
LDI R3, 0        ; null terminator

loop:
LD R2, 0x8000    ; load char
CMP R2, R3
JZ end
ST R2, 0xFF00    ; print char
INC R0
JMP loop

end:
HALT
