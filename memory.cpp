#include "memory.h"
#include "ppu.h" 

Memory::Memory() : ppu(nullptr) {
    for (int i = 0; i < 0x10000; i++) mem[i] = 0;
}

uint8_t Memory::read(uint16_t addr) {
    return mem[addr];
}

void Memory::write(uint16_t addr, uint8_t val) {
    if (addr < 0x8000) return;
    
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        if (ppu) ppu->writeVRAM(addr, val);
    }
    
    mem[addr] = val;
}