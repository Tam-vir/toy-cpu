// vga.cpp
#include "vga.hpp"
#include <iostream>

VGA::VGA() : framebuffer(FB_SIZE, 0) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        std::exit(1);
    }

    window = SDL_CreateWindow(
        "Toy VGA",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH * 2, HEIGHT * 2,  // scale x2
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        std::exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture  = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT
    );

    init_palette();
    clear();
}

VGA::~VGA() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void VGA::init_palette() {
    // Simple grayscale palette for now
    for (int i = 0; i < 256; i++) {
        uint8_t c = static_cast<uint8_t>(i);
        palette[i] = (0xFF << 24) | (c << 16) | (c << 8) | c; // RGBA
    }

    // A few nicer colors at low indices
    palette[1] = 0xFFFF0000; // red
    palette[2] = 0xFF00FF00; // green
    palette[3] = 0xFF0000FF; // blue
    palette[4] = 0xFFFFFF00; // yellow
    palette[5] = 0xFFFFFFFF; // white
    palette[6] = 0xFF00FFFF; // cyan
    palette[7] = 0xFFFF00FF; // pink
    palette[8] = 0xFF800000; // maroon
    palette[9] = 0xFF008000; // dark green
    palette[10] = 0xFF808000; // dark yellow

}

void VGA::clear(uint8_t color) {
    std::fill(framebuffer.begin(), framebuffer.end(), color);
}

void VGA::write(uint32_t addr, uint8_t value) {
    // Framebuffer MMIO
    if (addr >= 0x0000 && addr < FB_SIZE) {
        framebuffer[addr] = value;
        return;
    }

    // Control register
    if (addr == 0xFF10) {
        if (value == 1) {
            present();
        }
        return;
    }
}

uint8_t VGA::read(uint16_t addr) {
    if (addr >= 0x0000 && addr < FB_SIZE) {
        return framebuffer[addr];
    }

    return 0;
}

bool VGA::present() {
    uint32_t* pixels = nullptr;
    int pitch = 0;

    SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch);

    int row_pixels = pitch / 4; // convert bytes -> pixels per row

    for (int y = 0; y < HEIGHT; y++) {
        uint32_t* row = pixels + y * row_pixels; // start of row
        for (int x = 0; x < WIDTH; x++) {
            row[x] = palette[framebuffer[y * WIDTH + x]];
        }
    }

    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        }
    }

    return true;
}
