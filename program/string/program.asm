; Print Text from memory/data.txt
LDI R3, 0        ; null terminator
LDI R5, 0
LUI R5, 0x80
loop:
LDREG R2, R5    ; load char
CMP R2, R3
JZ end
ST R2, 0xFF00    ; print char
INC R5
JMP loop

end:
HALT
