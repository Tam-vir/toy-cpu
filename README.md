# Custom 8-bit CPU Emulator

A simple 8-bit CPU emulator in C++ with **memory-mapped I/O (MMIO)**, supporting basic instructions, string printing, and program loading from `.hex` files. This project demonstrates how CPUs, memory, and I/O interact at a low level.

---

## Features

* 4 general-purpose 8-bit registers (R0–R3)
* 16-bit program counter
* Zero flag (Z)
* Static 64KB memory
* Supported instructions:

| Opcode | Instruction | Description                                        |
| ------ | ----------- | -------------------------------------------------- |
| 0x00   | NOP         | No operation                                       |
| 0x01   | LDI r, imm  | Load immediate value into register                 |
| 0x02   | MOV r1, r2  | Copy value from one register to another            |
| 0x03   | ADD r1, r2  | Add two registers                                  |
| 0x04   | SUB r1, r2  | Subtract two registers                             |
| 0x05   | JMP addr    | Jump to address                                    |
| 0x06   | JZ addr     | Jump if zero flag set                              |
| 0x07   | CMP r1,r2   | Compare registers (sets Z)                         |
| 0x08   | LD r, addr  | Load from memory to register                       |
| 0x09   | ST r, addr  | Store register to memory (supports MMIO at 0xFF00) |
| 0x0A   | OUT r       | Print register as number                           |
| 0x0B   | INC r       | Increment register                                 |
| 0x0C   | JNZ addr    | Jump if zero flag not set                          |
| 0x0D   | PRTSTR addr | Print null-terminated string from memory           |
| 0xFF   | HALT        | Stop execution                                     |

* **Memory-mapped output**: Writing to `0xFF00` prints a character.
* Programs and strings can be loaded from files.

---

## Folder Structure

```
.
├── assembler
│   └── assembler.py       # Python assembler to convert .asm → .hex
├── cpu
│   ├── cpu.cpp            # CPU implementation
│   └── cpu.hpp            # CPU class definition
├── main.cpp               # Emulator main program
├── memory
│   └── data.txt           # String data to load into memory
├── program
│   ├── program.asm        # Example assembly program
│   └── program.hex        # Assembled bytecode
└── run.sh                 # Optional script to compile and run
```

---

## Getting Started

### Requirements

* C++17 compatible compiler (`g++`, `clang++`)
* Python 3 (for assembler)

### Compile the Emulator

```bash
g++ main.cpp cpu/cpu.cpp -o main
```

### Assemble a Program

```bash
python assembler/assembler.py program/program.asm program/program.hex
```

* Generates a `.hex` file for the emulator.
* Each instruction is separated by a line break in the hex file.

### Load Memory from Text File

Place your string in `memory/data.txt`. The emulator loads it into memory at `0x8000`.

Example `data.txt`:

```
Hello, World!
This is Tam :3
```

---

## Run the Emulator

```bash
./main program/program.hex memory/data.txt
```

* `program.hex`: the assembled instructions
* `data.txt`: string to load into memory

---

## Example Assembly Program (`program.asm`)

```asm
; Print string from memory using MMIO
LDI R0, 0        ; offset/index
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
```

---

## Notes

* The emulator currently **does not implement a real 6502**; it uses a simplified instruction set.
* Supports loops, jumps, and memory-mapped string printing.
* Designed for educational purposes and experimentation with assembly concepts.

---

## License

MIT License © Tamvir Shahabuddin
