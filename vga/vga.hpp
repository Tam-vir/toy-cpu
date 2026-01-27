// vga.hpp
#pragma once
#include <cstdint>
#include <vector>
#include <SDL2/SDL.h>

class VGA {
public:
    static constexpr int WIDTH  = 320;
    static constexpr int HEIGHT = 200;
    static constexpr int FB_SIZE = WIDTH * HEIGHT;

    VGA();
    ~VGA();

    void write(uint32_t addr, uint8_t value);  // MMIO write
    uint8_t read(uint16_t addr);               // MMIO read (optional)
    bool present();                            // Draw framebuffer
    void clear(uint8_t color = 0);

private:
    std::vector<uint8_t> framebuffer; // 8-bit color indices

    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture*  texture  = nullptr;

    uint32_t palette[256]; // RGBA colors

    void init_palette();
};
