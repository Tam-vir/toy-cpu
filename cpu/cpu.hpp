#pragma once
#include <cstdint>
#include <cstddef>
#include "../vga/vga.hpp"

class CPU {
public:
    CPU(VGA* vga);
    uint8_t fetch8();
    uint16_t fetch16();
    void load_program(const uint8_t* program, size_t size);
    void load_memory(const uint8_t* data, size_t size, uint16_t start_addr = 0);
    void run(bool debug = false);
    void exec(bool debug);

private:
    uint16_t reg[16];      // 16 general-purpose 16-bit registers
    uint16_t pc;
    bool Z;
    bool running;
    static uint8_t mem[65536];
    VGA* vga;
};
