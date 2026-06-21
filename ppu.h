#ifndef PPU_H
#define PPU_H
#include <cstdint>
#include <SDL2/SDL.h>

class PPU {
    uint8_t vram[0x2000];
    uint32_t pixels[160 * 144];
    SDL_Texture* texture;
public:
    PPU(SDL_Renderer* renderer);
    void writeVRAM(uint16_t addr, uint8_t val);
    uint8_t readVRAM(uint16_t addr);
    void update(int cycles, SDL_Renderer* renderer);
};
#endif