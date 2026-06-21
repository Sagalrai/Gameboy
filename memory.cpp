#include "memory.h"
#include "ppu.h"
#include "input.h"

Memory::Memory() : ppu(nullptr), input(nullptr), IE(0), IF(0) {
    for (int i = 0; i < 65536; i++) mem[i] = 0;
}

void Memory::loadCartridge(const std::vector<uint8_t>& data) { 
    rom = data; 
}

uint8_t Memory::read(uint16_t addr) {
    if (addr < 0x4000) {
        return (addr < rom.size()) ? rom[addr] : 0xFF;
    }
    if (addr >= 0x4000 && addr < 0x8000) {
        size_t bank_addr = (size_t)(currentRomBank * 0x4000) + (addr - 0x4000);
        return (bank_addr < rom.size()) ? rom[bank_addr] : 0xFF;
    }
    // Added hardware register hooks
    if (addr == 0xFF44) return ppu ? ppu->getScanline() : 0;
    if (addr == 0xFFFF) return IE;
    if (addr == 0xFF0F) return IF;
    if (addr == 0xFF00) return input ? input->getJoypadState() : 0xFF;
    return mem[addr];
}

void Memory::write(uint16_t addr, uint8_t val) {
    if (addr >= 0x2000 && addr <= 0x3FFF) {
        currentRomBank = (val == 0) ? 1 : val;
        return;
    }
    if (addr == 0xFFFF) { IE = val; return; }
    if (addr == 0xFF0F) { IF = val; return; }
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        if (ppu) ppu->writeVRAM(addr, val);
    }
    if (addr < 65536) mem[addr] = val;
}

void Memory::setInput(Input* i) { input = i; }
void Memory::setPPU(PPU* p) { ppu = p; }