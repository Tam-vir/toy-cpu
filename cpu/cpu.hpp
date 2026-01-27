#pragma once
#include <cstdint>
#include <cstddef>

class CPU {
public:
    CPU();
    uint8_t fetch8();
    uint16_t fetch16();
    void load_program(const uint8_t* program, size_t size);
    void load_memory(const uint8_t* data, size_t size, uint16_t start_addr = 0);
    void run(bool debug = false);
    void exec(bool debug);

private:
    uint8_t reg[4];
    uint16_t pc;
    bool Z;
    bool running;
    static uint8_t mem[65536];
};
