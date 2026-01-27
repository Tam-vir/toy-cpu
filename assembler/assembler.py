# assembler.py
import sys

# ----------------------------
# Opcode mapping
# ----------------------------
opcodes = {
    "NOP": 0x00,
    "LDI": 0x01,
    "MOV": 0x02,
    "ADD": 0x03,
    "SUB": 0x04,
    "JMP": 0x05,
    "JZ": 0x06,
    "CMP": 0x07,
    "LD": 0x08,
    "ST": 0x09,
    "OUT": 0x0A,
    "INC": 0x0B,
    "JNZ": 0x0C,
    "PRTSTR": 0x0D,
    "HALT": 0xFF,
}

# Instructions with 16-bit address operands
addr_instructions = ["JMP", "JZ", "JNZ", "LD", "ST", "PRTSTR"]

# ----------------------------
# Assemble function
# ----------------------------
def assemble(lines):
    # First pass: collect labels
    labels = {}
    pc = 0
    instructions = []

    for line in lines:
        line = line.split(';')[0].strip()  # remove comments
        if not line:
            continue

        if line.endswith(':'):  # label
            labels[line[:-1]] = pc
        else:
            tokens = line.split()
            mnemonic = tokens[0].upper()
            if mnemonic not in opcodes:
                raise ValueError(f"Unknown instruction: {mnemonic}")

            # Estimate instruction length
            if mnemonic in ["LDI", "MOV", "ADD", "SUB", "CMP"]:
                pc += 3  # opcode + 2 operands
            elif mnemonic in ["INC", "OUT"]:
                pc += 2  # opcode + 1 operand
            elif mnemonic in addr_instructions:
                pc += 3  # opcode + 16-bit address or 16-bit mem = 3 bytes
                if mnemonic in ["LD", "ST"]:
                    pc += 1  # extra byte for register
            else:  # NOP, HALT
                pc += 1

            instructions.append(tokens)

    # Second pass: generate bytes
    bytecode = []
    for tokens in instructions:
        mnemonic = tokens[0].upper()
        line_bytes = [opcodes[mnemonic]]

        if mnemonic == "LDI":
            line_bytes.append(int(tokens[1][1]))  # R0..R3
            line_bytes.append(int(tokens[2], 0))  # immediate
        elif mnemonic in ["MOV", "ADD", "SUB", "CMP"]:
            line_bytes.append(int(tokens[1][1]))
            line_bytes.append(int(tokens[2][1]))
        elif mnemonic in ["INC", "OUT"]:
            line_bytes.append(int(tokens[1][1]))
        elif mnemonic in ["JMP", "JZ", "JNZ", "PRTSTR"]:
            label = tokens[1]
            if label not in labels:
                raise ValueError(f"Unknown label: {label}")
            addr = labels[label]
            line_bytes.append(addr & 0xFF)
            line_bytes.append((addr >> 8) & 0xFF)
        elif mnemonic in ["LD", "ST"]:
            line_bytes.append(int(tokens[1][1]))  # register
            addr = tokens[2]
            if addr.startswith("0x"):
                addr = int(addr, 16)
            elif addr.isdigit():
                addr = int(addr)
            elif addr in labels:
                addr = labels[addr]
            else:
                raise ValueError(f"Unknown address: {addr}")
            line_bytes.append(addr & 0xFF)
            line_bytes.append((addr >> 8) & 0xFF)
        # NOP and HALT have no operands

        bytecode.append(line_bytes)  # keep each instruction as a separate list

    return bytecode

# ----------------------------
# Write HEX file with line breaks
# ----------------------------
def write_hex_file(bytecode, filename):
    with open(filename, "w") as f:
        for instr_bytes in bytecode:
            f.write(' '.join(f"{b:02X}" for b in instr_bytes) + '\n')

# ----------------------------
# Example usage
# ----------------------------
if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python assembler.py input.asm output.hex")
        sys.exit(1)

    with open(sys.argv[1], "r") as f:
        lines = f.readlines()

    bytecode = assemble(lines)
    write_hex_file(bytecode, sys.argv[2])
    total_bytes = sum(len(instr) for instr in bytecode)
    print(f"Generated {sys.argv[2]} with {total_bytes} bytes")
