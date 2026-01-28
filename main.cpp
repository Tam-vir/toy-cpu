#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "cpu/cpu.hpp"
#include "vga/vga.hpp"
// Load a hex program into a vector
bool load_hex_program(const std::string& filename, std::vector<uint8_t>& program) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open " << filename << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string byte_str;
        while (iss >> byte_str) {
            uint8_t byte = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
            program.push_back(byte);
        }
    }

    return true;
}

// Load a text file into memory starting at start_addr
bool load_text_to_memory(const std::string& filename, CPU& cpu, uint16_t start_addr) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << filename << "\n";
        return false;
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
    buffer.push_back(0x00); // Null terminator for string

    cpu.load_memory(buffer.data(), buffer.size(), start_addr);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <hex_program>.hex <string_file>.txt\n";
        return 1;
    }
    VGA vga;
    CPU cpu(&vga);

    // Load string from text file into memory at 0x8000
    if (!load_text_to_memory(argv[2], cpu, 0x8000)) return 1;

    // Load hex program
    std::vector<uint8_t> program;
    if (!load_hex_program(argv[1], program)) return 1;

    cpu.load_program(program.data(), program.size());
    cpu.run();
    bool running = true;
    while (running) {
        if(cpu.vga_present){
            running = vga.present();
        }
        else
            running = false;
        SDL_Delay(16);
    }

    return 0;
}
