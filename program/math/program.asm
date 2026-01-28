; y = 20x + 7
; x = 5

LDI R0, 5       ; x = 5

LDI R2, 20
LDI R3, 7

; Calculation

MUL R0, R2
ADD R0, R3

OUT R0

HALT
