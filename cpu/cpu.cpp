#include "cpu.hpp"
#include <iostream>

uint8_t CPU::mem[65536] = {0};

CPU::CPU(VGA *vga_ptr) : vga(vga_ptr)
{
    pc = 0;
    Z = false;
    running = true;
    for (int i = 0; i < 16; i++)
        reg[i] = 0;
}

uint8_t CPU::fetch8() { return mem[pc++]; }

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
    while (running)
        exec(debug);
}

void CPU::exec(bool debug)
{
    uint8_t opcode = fetch8();
    if (debug)
        std::cout << "Opcode: " << (int)opcode << "\n";

    switch (opcode)
    {
    case 0x00: break; // NOP

    case 0x01: { // LDI r, imm16
        uint8_t r = fetch8();
        uint16_t imm = fetch16();
        if (r < 16) reg[r] = imm;
        Z = (imm == 0);
        break;
    }

    case 0x02: { // MOV r1,r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            reg[r1] = reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x03: { // ADD r1,r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            reg[r1] += reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x04: { // SUB r1,r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            reg[r1] -= reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x05: pc = fetch16(); break; // JMP

    case 0x06: { uint16_t addr = fetch16(); if (Z) pc = addr; break; } // JZ
    case 0x0C: { uint16_t addr = fetch16(); if (!Z) pc = addr; break; } // JNZ

    case 0x07: { // CMP r1,r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            Z = (reg[r1] == reg[r2]);
        break;
    }

    case 0x08: { // LD r, addr
        uint8_t r = fetch8();
        uint16_t addr = fetch16();
        if (r < 16)
            reg[r] = mem[addr];
        Z = (reg[r] == 0);
        break;
    }

    case 0x09: { // ST r, addr
        uint8_t r = fetch8();
        uint16_t addr = fetch16();

        if (addr == 0xFF00)
            std::cout << (char)reg[r];
        else if (addr == 0xFF10)
            vga->present();
        else if (addr < 0xF000 + VGA::FB_SIZE)
            vga->write(0xF000 + addr, reg[r] & 0xFF);
        else
            mem[addr] = reg[r] & 0xFF;

        break;
    }

    case 0x0A: { // OUT r
        uint8_t r = fetch8();
        if (r < 16)
            std::cout << reg[r] << "\n";
        break;
    }

    case 0x0B: { // INC r
        uint8_t r = fetch8();
        if (r < 16) reg[r]++;
        Z = (reg[r] == 0);
        break;
    }

    case 0x10: { // MUL r1,r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            reg[r1] = reg[r1] * reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x11: { // DIV r1,r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            reg[r1] = (reg[r2] == 0) ? 0 : reg[r1] / reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x12: { // MOD r1,r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 16 && r2 < 16)
            reg[r1] = (reg[r2] == 0) ? 0 : reg[r1] % reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x0D: { // PRTSTR addr
        uint16_t addr = fetch16();
        while (mem[addr] != 0x00) {
            reg[0] = mem[addr];
            std::cout << (char)reg[0];
            addr++;
        }
        break;
    }

    case 0xFF: running = false; break;

    default:
        std::cerr << "Unknown opcode: " << std::hex << (int)opcode << "\n";
        running = false;
        break;
    }
}

void CPU::load_memory(const uint8_t *data, size_t size, uint16_t start_addr)
{
    for (size_t i = 0; i < size; i++)
        mem[start_addr + i] = data[i];
}
