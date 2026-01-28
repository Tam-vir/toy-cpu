#pragma once
#include <cstdint>
#include <cstddef>
#include "../vga/vga.hpp"

class CPU {
public:
    CPU(VGA* vga);
    ~CPU();
    uint8_t fetch8();
    uint16_t fetch16();
    void load_program(const uint8_t* program, size_t size);
    void load_memory(const uint8_t* data, size_t size, uint16_t start_addr = 0);
    void run(bool debug = false);
    void exec(bool debug);
    bool vga_present = false;

private:
    uint16_t reg[16];
    uint16_t pc;
    uint16_t vram_addr = 0;
    bool Z;
    bool running;
    static uint8_t mem[65536];
    static uint8_t vram[65536];
    VGA* vga;
};
