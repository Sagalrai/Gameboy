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
    for (int i = 0; i < 256; i++) {
        int x = (i % 16) * 10;
        int y = (i / 16) * 10;
        
        uint8_t vram_val = readVRAM(0x8000 + i);
        uint32_t color = (vram_val == 0) ? 0xFF000000 : 0xFF00FF00; 

        for(int dy = 0; dy < 10; dy++) {
            for(int dx = 0; dx < 10; dx++) {
                if (x+dx < 160 && y+dy < 144) {
                    pixels[(y+dy)*160 + (x+dx)] = color;
                }
            }
        }
    }

    SDL_UpdateTexture(texture, NULL, pixels, 160 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}