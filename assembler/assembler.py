#!/usr/bin/env python3
import sys
import re

# ----------------------------
# Opcode mapping (match your CPU)
# ----------------------------
opcodes = {
    "NOP":    0x00,
    "LDI":    0x01,   # LDI r, imm8 (low byte)
    "LUI":    0x13,   # LUI r, imm8 (high byte)
    "MOV":    0x02,
    "ADD":    0x03,
    "SUB":    0x04,
    "JMP":    0x05,
    "JZ":     0x06,
    "CMP":    0x07,
    "LD":     0x08,
    "ST":     0x09,
    "OUT":    0x0A,
    "INC":    0x0B,
    "JNZ":    0x0C,
    "PRTSTR": 0x0D,
    "MUL":    0x0E,
    "DIV":    0x0F,
    "MOD":    0x10,
    "LDREG":  0x11,  # LD r1, r2
    "STREG":  0x12,  # ST r1, r2
    "JMPR":   0x14,
    "JZR":    0x15,
    "JNZR":   0x16,
    "HALT":   0xFF,
}

# ----------------------------
# Helpers
# ----------------------------
def parse_register(tok):
    tok = tok.upper().strip()
    if not re.fullmatch(r"R\d+", tok):
        raise ValueError(f"Invalid register: {tok}")
    r = int(tok[1:])
    if not (0 <= r <= 15):
        raise ValueError(f"Register out of range: {tok}")
    return r

def parse_number(tok):
    tok = tok.strip()
    if tok.startswith("0x") or tok.startswith("0X"):
        return int(tok, 16)
    if tok.isdigit():
        return int(tok)
    return None  # label

# ----------------------------
# First pass: collect labels
# ----------------------------
def collect_labels(lines):
    labels = {}
    pc = 0
    for line in lines:
        line = line.split(";")[0].strip()
        if not line:
            continue
        if line.endswith(":"):
            label = line[:-1]
            labels[label] = pc
            continue

        tokens = re.split(r"[,\s]+", line)
        op = tokens[0].upper()
        if op not in opcodes:
            raise ValueError(f"Unknown opcode: {op}")

        pc += 1  # opcode
        if op in ("NOP", "HALT"):
            pass
        elif op in ("OUT", "INC", "JMPR", "JZR", "JNZR"):
            pc += 1
        elif op == "LUI":
            pc += 2
        elif op == "LDI":
            pc += 2
        elif op in ("MOV","ADD","SUB","CMP","MUL","DIV","MOD","LDREG","STREG"):
            pc += 2
        elif op in ("JMP","JZ","JNZ","PRTSTR"):
            pc += 2
        elif op in ("LD","ST"):
            if len(tokens) != 3:
                raise ValueError(f"Bad operand count: {line}")
            if tokens[2].upper().startswith("R"):
                pc += 2
            else:
                pc += 3
        else:
            raise ValueError(f"Unhandled opcode in pass1: {op}")

    return labels

# ----------------------------
# Second pass: emit bytes
# ----------------------------
def assemble(lines):
    labels = collect_labels(lines)
    out = []

    def emit8(v):
        out.append(v & 0xFF)

    def emit16(v):
        emit8(v & 0xFF)
        emit8((v >> 8) & 0xFF)

    for line in lines:
        line = line.split(";")[0].strip()
        if not line or line.endswith(":"):
            continue

        tokens = [t for t in re.split(r"[,\s]+", line) if t]
        op = tokens[0].upper()
        if op not in opcodes:
            raise ValueError(f"Unknown opcode: {op}")

        emit8(opcodes[op])

        if op in ("NOP", "HALT"):
            continue

        elif op in ("OUT","INC","JMPR","JZR","JNZR"):
            r = parse_register(tokens[1])
            emit8(r)

        elif op == "LUI":
            r = parse_register(tokens[1])
            imm = parse_number(tokens[2])
            if imm is None:
                raise ValueError(f"Invalid immediate: {tokens[2]}")
            emit8(r)
            emit8(imm)

        elif op == "LDI":
            r = parse_register(tokens[1])
            imm = parse_number(tokens[2])
            if imm is None:
                raise ValueError(f"Invalid immediate: {tokens[2]}")
            emit8(r)
            emit8(imm)

        elif op in ("MOV","ADD","SUB","CMP","MUL","DIV","MOD","LDREG","STREG"):
            r1 = parse_register(tokens[1])
            r2 = parse_register(tokens[2])
            emit8(r1)
            emit8(r2)

        elif op in ("JMP","JZ","JNZ","PRTSTR"):
            addr_tok = tokens[1]
            addr = parse_number(addr_tok)
            if addr is None:
                if addr_tok not in labels:
                    raise ValueError(f"Unknown label or address: {addr_tok}")
                addr = labels[addr_tok]
            emit16(addr)

        elif op in ("LD","ST"):
            r1 = parse_register(tokens[1])
            emit8(r1)
            op2 = tokens[2]
            if op2.upper().startswith("R"):
                r2 = parse_register(op2)
                emit8(r2)
            else:
                addr = parse_number(op2)
                if addr is None:
                    if op2 not in labels:
                        raise ValueError(f"Unknown label or address: {op2}")
                    addr = labels[op2]
                emit16(addr)

        else:
            raise ValueError(f"Unhandled opcode: {op}")

    return out

# ----------------------------
# Entry point
# ----------------------------
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: assembler.py input.asm output.hex")
        sys.exit(1)

    with open(sys.argv[1],"r") as f:
        lines = f.readlines()

    bytecode = assemble(lines)

    with open(sys.argv[2],"w") as f:
        for i,b in enumerate(bytecode):
            f.write(f"{b:02X}")
            if i+1 < len(bytecode):
                f.write(" ")

    print(f"Generated {sys.argv[2]} with {len(bytecode)} bytes")
