#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>

class PPU; // Forward declaration

class Memory {
    uint8_t mem[65536];
    PPU* ppu;
public:
    Memory();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    void setPPU(PPU* p) { ppu = p; } // Add this
};
#endif