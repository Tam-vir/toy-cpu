; y = 20x + 7
; x = 5

LDI R1, 5       ; x = 5

LDI R2, 20
LDI R3, 7

; Calculation

MUL R1, R2
ADD R1, R3

OUT R1

HALT
