#include "ppu.h"

PPU::PPU(SDL_Renderer* renderer) {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                SDL_TEXTUREACCESS_STREAMING, 160, 144);
    
    for (int i = 0; i < 160 * 144; i++) {
        pixels[i] = 0;
    }
}

void PPU::writeVRAM(uint16_t addr, uint8_t val) {
    vram[addr - 0x8000] = val;
}

uint8_t PPU::readVRAM(uint16_t addr) {
    return vram[addr - 0x8000];
}

void PPU::update(int cycles, SDL_Renderer* renderer) {
    static int color = 0;
    color += 2; 

    for (int i = 0; i < 160 * 144; i++) {
        pixels[i] = 0xFF000000 | ((color % 255) << 16); 
    }

    SDL_UpdateTexture(texture, NULL, pixels, 160 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}