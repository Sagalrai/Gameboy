#include "cpu.h"
#include "memory.h"
#include <iostream>

GameBoyCPU::GameBoyCPU(Memory* mem) : memory(mem), pc(0x0100), sp(0xFFFE) {
    af = bc = de = hl = 0;
}

uint8_t GameBoyCPU::read(uint16_t addr) { return memory->read(addr); }
void GameBoyCPU::write(uint16_t addr, uint8_t val) { memory->write(addr, val); }

int GameBoyCPU::step() {
    uint8_t op = read(pc++);
    switch (op) {
        case 0x00: return 4;
        case 0x3E: a = read(pc++); return 8;
        case 0x06: b = read(pc++); return 8;
        case 0x78: a = b; return 4;
        case 0x80: a += b; return 4;
        case 0x76: return 0;
        default: return 0;
    }
}

void GameBoyCPU::debug() {
    std::cout << "A=" << (int)a << " B=" << (int)b << " PC=" << pc << "\n";
}