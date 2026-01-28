//cpu.cpp
#include "cpu.hpp"
#include <iostream>

uint8_t CPU::mem[65536] = {0};
uint8_t CPU::vram[65536] = {0};

CPU::CPU(VGA *vga_ptr) : vga(vga_ptr)
{
    pc = 0;
    Z = false;
    running = true;
    for (int i = 0; i < 16; i++)
        reg[i] = 0;
}

uint8_t CPU::fetch8()
{
    return mem[pc++];
}

uint16_t CPU::fetch16()
{
    uint8_t lo = fetch8();
    uint8_t hi = fetch8();
    return (hi << 8) | lo;
}

void CPU::load_program(const uint8_t *program, size_t size)
{
    for (size_t i = 0; i < size; i++)
        mem[i] = program[i];
}

void CPU::run(bool debug)
{
    while (running){
        exec(debug);
        std::cin;
    }
    
}

void CPU::exec(bool debug)
{
    uint8_t opcode = fetch8();
    if (debug)
        std::cout << "\nExecuting opcode: 0x" << std::hex << (int)opcode << std::dec << "\n";

    switch (opcode)
    {
    case 0x00:
        break; // NOP

    case 0x01: // LDI r, imm8 (low byte)
    {
        uint8_t r = fetch8();
        uint8_t imm = fetch8();
        if (r > 0 && r < 16)  // Skip if r == 0 (zero register)
            reg[r] = (reg[r] & 0xFF00) | imm;
        Z = (r == 0 ? true : (reg[r] == 0));  // R0 is always zero, so Z should be true
        break;
    }

    case 0x13: // LUI r, imm8 (upper byte)  🔥 NEW
    {
        uint8_t r = fetch8();
        uint8_t imm = fetch8();
        if (r > 0 && r < 16)  // Skip if r == 0 (zero register)
            reg[r] = (imm << 8) | (reg[r] & 0x00FF);
        Z = (r == 0 ? true : (reg[r] == 0));
        break;
    }

    case 0x02: // MOV r1, r2
    {
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 > 0 && r1 < 16 && r2 < 16)  // Can't write to R0
            reg[r1] = reg[r2];
        Z = (r1 == 0 ? true : (reg[r1] == 0));
        break;
    }

    case 0x03: // ADD r1, r2
    {
        uint8_t r1 = fetch8(), r2 = fetch8();
        if (r1 > 0 && r1 < 16 && r2 < 16)  // Can't write to R0
        {
            reg[r1] += reg[r2];
            if (reg[r1] > 0xFFFF)
            {
                std::cerr << "Error: ADD overflow\n";
                running = false;
                break;
            }
        }
        Z = (r1 == 0 ? true : (reg[r1] == 0));
        break;
    }

    case 0x04: // SUB r1, r2
    {
        uint8_t r1 = fetch8(), r2 = fetch8();
        if (r1 > 0 && r1 < 16 && r2 < 16)  // Can't write to R0
        {
            if (reg[r1] < reg[r2])
            {
                std::cerr << "Error: SUB underflow\n";
                running = false;
                break;
            }
            reg[r1] -= reg[r2];
        }
        Z = (r1 == 0 ? true : (reg[r1] == 0));
        break;
    }

    case 0x05: // JMP addr
        pc = fetch16();
        break;

    case 0x06: // JZ addr
    {
        uint16_t addr = fetch16();
        if (Z)
            pc = addr;
        break;
    }

    case 0x07: // CMP r1, r2
    {
        uint8_t r1 = fetch8(), r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            Z = (reg[r1] == reg[r2]);
        break;
    }

    case 0x08: // LD r, addr
    {
        uint8_t r = fetch8();
        uint16_t addr = fetch16();
        if (r > 0 && r < 16)  // Can't write to R0
            reg[r] = mem[addr] | (mem[addr + 1] << 8);
        Z = (r == 0 ? true : (reg[r] == 0));
        break;
    }

    case 0x09: // ST r, addr
    {
        uint8_t r = fetch8();
        uint16_t addr = fetch16();
        uint16_t val = (r == 0) ? 0 : reg[r];  // R0 always reads as 0

        // --- Output as character ---
        if (addr == 0xFF00)
        {
            std::cout << static_cast<char>(val & 0xFF);
        }
        // --- VRAM address low byte ---
        else if (addr == 0xFF01)
        {
            vram_addr = (vram_addr & 0x0000) | (val);
        }
        // --- VRAM address high byte ---
        else if (addr == 0xFF02)
        {
            vram_addr = (vram_addr) | ((val & 0xFF00));
        }
        // --- VRAM data write ---
        else if (addr == 0xFF03)
        {
            vram[vram_addr] = val;
            vga->write(vram_addr, vram[vram_addr]);
        }
        // --- Trigger VGA present ---
        else if (addr == 0xFF06)
        {
            vga_present = true;
        }
        // --- Normal memory ---
        else
        {
            mem[addr] = val & 0xFF;
            mem[addr + 1] = (val >> 8) & 0xFF;
        }

        break;
    }

    case 0x0A:
    { // OUT
        // Output value in r
        uint8_t r = fetch8();
        std::cout << ((r == 0) ? 0 : reg[r]) << " ";  // R0 outputs 0
        break;
    }
    case 0x0B: // INC r
    {
        uint8_t r = fetch8();
        if (r > 0 && r < 16)  // Can't write to R0
            reg[r]++;
        Z = (r == 0 ? true : (reg[r] == 0));
        break;
    }

    case 0x0C: // JNZ addr
    {
        uint16_t addr = fetch16();
        if (!Z)
            pc = addr;
        break;
    }
    case 0x0D:
    { // PRTSTR addr
        uint16_t addr = fetch16();
        while (mem[addr] != 0x00)
        {
            // Use R0 as zero for output
            std::cout << (char)mem[addr];
            addr++;
        }
        break;
    }

    case 0x0E: // MUL r1, r2
    {
        uint8_t r1 = fetch8(), r2 = fetch8();
        if (r1 > 0 && r1 < 16 && r2 < 16)  // Can't write to R0
        {
            uint32_t result = reg[r1] * reg[r2];
            if (result > 0xFFFF)
            {
                std::cout << "MUL - r1 : " << reg[r1] << " r2: " << reg[r2] << " : " << result << std::endl;
                std::cerr << "Error: MUL overflow\n";
                running = false;
                break;
            }
            reg[r1] = (uint16_t)result;
        }
        Z = (r1 == 0 ? true : (reg[r1] == 0));
        break;
    }

    case 0x0F: // DIV r1, r2
    {
        uint8_t r1 = fetch8(), r2 = fetch8();
        if (r1 > 0 && r1 < 16 && r2 < 16)  // Can't write to R0
        {
            if (reg[r2] == 0)
            {
                std::cerr << "Error: Division by zero\n";
                running = false;
                break;
            }
            reg[r1] /= reg[r2];
        }
        Z = (r1 == 0 ? true : (reg[r1] == 0));
        break;
    }

    case 0x10: // MOD r1, r2
    {
        uint8_t r1 = fetch8(), r2 = fetch8();
        if (r1 > 0 && r1 < 16 && r2 < 16)  // Can't write to R0
        {
            if (reg[r2] == 0)
            {
                std::cerr << "Error: Modulo by zero\n";
                running = false;
                break;
            }
            reg[r1] %= reg[r2];
        }
        Z = (r1 == 0 ? true : (reg[r1] == 0));
        break;
    }

    case 0x11: // LD r1, r2 (load from memory address in r2)
    {
        uint8_t r1 = fetch8(), r2 = fetch8();
        if (r1 > 0 && r1 < 16 && r2 < 16)  // Can't write to R0
            reg[r1] = mem[reg[r2]] | (mem[reg[r2] + 1] << 8);
        Z = (r1 == 0 ? true : (reg[r1] == 0));
        break;
    }

    case 0x12: // ST r1, r2  (store reg[r1] -> [reg[r2]])
    {
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();

        if (r1 >= 16 || r2 >= 16)
            break;

        uint16_t addr = reg[r2];
        uint16_t val = (r1 == 0) ? 0 : reg[r1];  // R0 always reads as 0

        // --- Output as character ---
        if (addr == 0xFF00)
        {
            std::cout << static_cast<char>(val & 0xFF);
        }
        // --- VRAM address low byte ---
        else if (addr == 0xFF01)
        {
            vram_addr = (vram_addr & 0x0000) | (val & 0xFF);
        }
        // --- VRAM address high byte ---
        else if (addr == 0xFF02)
        {
            vram_addr = (vram_addr) | ((val & 0xFF00));
        }
        // --- VRAM data write ---
        else if (addr == 0xFF03)
        {
            vram[vram_addr] = val;
            vga->write(vram_addr, vram[vram_addr]);
        }
        // --- Trigger VGA present ---
        else if (addr == 0xFF06)
        {
            vga_present = true;
        }
        // --- Normal RAM ---
        else
        {
            mem[addr] = val & 0xFF;
            mem[addr + 1] = (val >> 8) & 0xFF;
        }

        break;
    }

    case 0x14: // JMPR r
    {
        uint8_t r = fetch8();
        if (r < 16)
            pc = (r == 0) ? 0 : reg[r];  // Jump to 0 if r == 0
        break;
    }

    case 0x15: // JZR r
    {
        uint8_t r = fetch8();
        if (Z && r < 16)
            pc = (r == 0) ? 0 : reg[r];  // Jump to 0 if r == 0
        break;
    }

    case 0x16: // JNZR r
    {
        uint8_t r = fetch8();
        if (!Z && r < 16)
            pc = (r == 0) ? 0 : reg[r];  // Jump to 0 if r == 0
        break;
    }

    case 0xFF: // HALT
        running = false;
        break;

    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << "\n";
        running = false;
        break;
    }
    
    // Ensure R0 is always zero after any operation
    reg[0] = 0;
}

void CPU::load_memory(const uint8_t *data, size_t size, uint16_t start_addr)
{
    for (size_t i = 0; i < size; i++)
        mem[start_addr + i] = data[i];
}

CPU::~CPU(){
    std::cout << "done\n";
}