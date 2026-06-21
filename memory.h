#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>
#include <vector>
#include "input.h"

class PPU;

class Memory {
public:
    Memory();
    void loadCartridge(const std::vector<uint8_t>& data);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    void setPPU(PPU* p);
    void setInput(Input* i);

private:
    uint8_t mem[65536];
    std::vector<uint8_t> rom;
    int currentRomBank = 1;
    PPU* ppu;
    Input* input;
    uint8_t IE;
    uint8_t IF;
};
#endif