#include "cpu.h"
#include "memory.h"
#include <iostream>

GameBoyCPU::GameBoyCPU(Memory* mem) : memory(mem), pc(0x0100), sp(0xFFFE) {
    af = bc = de = hl = 0;
}

uint8_t GameBoyCPU::read(uint16_t addr) { return memory->read(addr); }
void GameBoyCPU::write(uint16_t addr, uint8_t val) { memory->write(addr, val); }

void GameBoyCPU::setFlags(uint8_t z, uint8_t n, uint8_t h, uint8_t c) {
    f = (z << 7) | (n << 6) | (h << 5) | (c << 4);
}

int GameBoyCPU::step() {
    uint8_t op = read(pc++);
    switch (op) {
        case 0x00: return 4;
        case 0x01: bc = (read(pc+1) << 8) | read(pc); pc += 2; return 12;
        case 0x05: b--; setFlags(b == 0, 1, 0, (f & 0x10)); return 4;
        case 0x06: b = read(pc++); return 8;
        case 0x08: {
            uint16_t addr = (read(pc+1) << 8) | read(pc); pc += 2;
            write(addr, sp & 0xFF); write(addr + 1, sp >> 8); return 20;
        }
        case 0x0C: c++; setFlags(c == 0, 0, 0, (f & 0x10)); return 4;
        case 0x0D: c--; setFlags(c == 0, 1, 0, (f & 0x10)); return 4;
        case 0x0E: c = read(pc++); return 8;
        case 0x11: de = (read(pc+1) << 8) | read(pc); pc += 2; return 12;
        case 0x15: d--; setFlags(d == 0, 1, 0, (f & 0x10)); return 4;
        case 0x16: d = read(pc++); return 8;
        case 0x1A: a = read(de); return 8;
        case 0x1E: e = read(pc++); return 8;
        case 0x20: {
            int8_t offset = (int8_t)read(pc++);
            if (!(f & 0x80)) { pc += offset; return 12; }
            return 8;
        }
        case 0x21: {
            uint8_t lo = read(pc++);
            uint8_t hi = read(pc++);
            hl = (hi << 8) | lo;
            return 12;
        }
        case 0x23: hl++; return 8;
        case 0x31: sp = (read(pc+1) << 8) | read(pc); pc += 2; return 12;
        case 0x32: write(hl--, a); return 8;
        case 0x3E: a = read(pc++); return 8;
        case 0x76: return 4;
        case 0x77: write(hl, a); return 8;
        case 0x80: { uint16_t res = a + b; setFlags((res & 0xFF) == 0, 0, 0, res > 0xFF); a = (uint8_t)res; return 4; }
        case 0x81: { uint16_t res = a + c; setFlags((res & 0xFF) == 0, 0, 0, res > 0xFF); a = (uint8_t)res; return 4; }
        case 0x87: { uint16_t res = a + a; setFlags((res & 0xFF) == 0, 0, 0, res > 0xFF); a = (uint8_t)res; return 4; }
        case 0x90: { uint8_t res = a - b; setFlags(res == 0, 1, 0, a < b); a = res; return 4; }
        case 0xA0: { a &= b; setFlags(a == 0, 0, 1, 0); return 4; }
        case 0xA8: { a ^= b; setFlags(a == 0, 0, 0, 0); return 4; }
        case 0xB0: { a |= b; setFlags(a == 0, 0, 0, 0); return 4; }
        case 0xAF: { a = 0; setFlags(1, 0, 0, 0); return 4; }
        case 0xC3: pc = (read(pc+1) << 8) | read(pc); return 16;
        default: return 4;
    }
}