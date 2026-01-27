#include "cpu.hpp"
#include <iostream>

uint8_t CPU::mem[65536] = {0};

CPU::CPU()
{
    pc = 0;
    Z = false;
    running = true;
    for (int i = 0; i < 4; i++)
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
    {
        mem[i] = program[i];
    }
}

void CPU::run(bool debug)
{
    while (running)
    {
        exec(debug);
    }
}

void CPU::exec(bool debug)
{
    uint8_t opcode = fetch8();
    if (debug)
        std::cout << "Executing opcode: " << (int)opcode << std::endl;
    switch (opcode)
    {
    case 0x00: // NOP
        break;

    case 0x01:
    { // LDI r, imm
        uint8_t r = fetch8();
        uint8_t imm = fetch8();
        if (r < 4)
            reg[r] = imm;
        Z = (imm == 0);
        break;
    }

    case 0x02:
    { // MOV r1, r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 4 && r2 < 4)
            reg[r1] = reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x03:
    { // ADD r1, r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 4 && r2 < 4)
            reg[r1] += reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x04:
    { // SUB r1, r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 4 && r2 < 4)
            reg[r1] -= reg[r2];
        Z = (reg[r1] == 0);
        break;
    }

    case 0x05:
    { // JMP addr
        uint16_t addr = fetch16();
        pc = addr;
        break;
    }

    case 0x06:
    { // JZ addr
        uint16_t addr = fetch16();
        if (Z)
            pc = addr;
        break;
    }

    case 0x07:
    { // CMP r1, r2
        uint8_t r1 = fetch8();
        uint8_t r2 = fetch8();
        if (r1 < 4 && r2 < 4)
            Z = (reg[r1] == reg[r2]);
        break;
    }

    case 0x08:
    { // LD r, addr
        uint8_t r = fetch8();
        uint16_t addr = fetch16();
        if (r < 4)
        {
            // Use R0 as an index into memory
            reg[r] = mem[addr + reg[0]];
        }
        Z = (reg[r] == 0);
        break;
    }

    case 0x09:
    { // ST r, addr
        uint8_t r = fetch8();
        uint16_t addr = fetch16();

        if (addr == 0xFF00)
        {
            // Memory-mapped output → print as character
            std::cout << (char)reg[r];
        }
        else
        {
            mem[addr] = reg[r];
        }
        break;
    }

    case 0x0A:
    { // OUT r
        uint8_t r = fetch8();
        if (r < 4)
            std::cout << (int)reg[r] << std::endl;
        break;
    }
    case 0x0B:
    { // INC r
        uint8_t r = fetch8();
        if (r < 4)
            reg[r]++;
        Z = (reg[r] == 0);
        break;
    }

    case 0x0C:
    { // JNZ addr
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
            reg[0] = mem[addr]; // load character into R0

            // simulate ST opcode execution
            std::cout << (char)reg[0];

            addr++;
        }
        break;
    }

    case 0xFF: // HALT
        running = false;
        break;

    default:
        std::cerr << "Unknown opcode: " << std::hex << (int)opcode << "\n";
        running = false;
        break;
    }
}
void CPU::load_memory(const uint8_t *data, size_t size, uint16_t start_addr)
{
    for (size_t i = 0; i < size; i++)
    {
        mem[start_addr + i] = data[i];
    }
}