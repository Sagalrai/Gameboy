#include "cpu.h"
#include "memory.h"
#include <iostream>
#include <iomanip>

GameBoyCPU::GameBoyCPU(Memory* mem) : memory(mem), pc(0x0100), sp(0xFFFE), IME(false) {
    af = bc = de = hl = 0;
}

uint8_t GameBoyCPU::read(uint16_t addr) { return memory->read(addr); }
void GameBoyCPU::write(uint16_t addr, uint8_t val) { memory->write(addr, val); }

void GameBoyCPU::setFlags(uint8_t z, uint8_t n, uint8_t h, uint8_t c) {
    f = (z << 7) | (n << 6) | (h << 5) | (c << 4);
}

void GameBoyCPU::handleInterrupts() {
    if (!IME) return;

    uint8_t enabled = memory->read(0xFFFF);
    uint8_t requested = memory->read(0xFF0F);
    uint8_t fire = enabled & requested;

    if (fire > 0) {
        IME = false;
        if (fire & 0x01) {
            memory->write(0xFF0F, requested & ~0x01);
            write(--sp, (pc >> 8) & 0xFF);
            write(--sp, pc & 0xFF);
            pc = 0x0040;
        }
    }
}

int GameBoyCPU::handlePrefixCB() {
    uint8_t op = read(pc++);
    switch (op) {
        case 0x06: { 
            uint8_t bit = (read(hl) >> 0) & 1;
            setFlags(bit == 0, 0, 1, (f & 0x10));
            return 16;
        }
        case 0x11: {
            uint8_t res = (c >> 7) | (c << 1);
            c = res;
            setFlags(res == 0, 0, 0, (c & 1));
            return 8;
        }
        case 0x37: { a = (a >> 4) | (a << 4); setFlags(a == 0, 0, 0, 0); return 8; }
        case 0x7C: {
            uint8_t bit = (h >> 7) & 1;
            setFlags(bit == 0, 0, 1, (f & 0x10));
            return 8;
        }
        default: 
            std::cout << "Unknown CB Opcode: 0x" << std::hex << (int)op << std::endl;
            return 8;
    }
}

int GameBoyCPU::step() {
    uint16_t currentPC = pc;
    uint8_t op = read(pc++);
    int cycles = 0;

    switch (op) {
        case 0x00: cycles = 4; break;
        case 0x01: bc = (read(pc+1) << 8) | read(pc); pc += 2; cycles = 12; break;
        case 0x02: write(bc, a); cycles = 8; break;
        case 0x05: b--; setFlags(b == 0, 1, 0, (f & 0x10)); cycles = 4; break;
        case 0x06: b = read(pc++); cycles = 8; break;
        case 0x08: {
            uint16_t addr = (read(pc+1) << 8) | read(pc); pc += 2;
            write(addr, sp & 0xFF); write(addr + 1, sp >> 8); cycles = 20; break;
        }
        case 0x0A: a = read(bc); cycles = 8; break;
        case 0x0C: c++; setFlags(c == 0, 0, 0, (f & 0x10)); cycles = 4; break;
        case 0x0D: c--; setFlags(c == 1, 1, 0, (f & 0x10)); cycles = 4; break;
        case 0x0E: c = read(pc++); cycles = 8; break;
        case 0x11: de = (read(pc+1) << 8) | read(pc); pc += 2; cycles = 12; break;
        case 0x12: write(de, a); cycles = 8; break;
        case 0x15: d--; setFlags(d == 0, 1, 0, (f & 0x10)); cycles = 4; break;
        case 0x16: d = read(pc++); cycles = 8; break;
        case 0x18: { int8_t off = (int8_t)read(pc++); pc += off; cycles = 12; break; }
        case 0x1A: a = read(de); cycles = 8; break;
        case 0x1E: e = read(pc++); cycles = 8; break;
        case 0x20: {
            int8_t offset = (int8_t)read(pc++);
            if (!(f & 0x80)) { pc += offset; cycles = 12; } else { cycles = 8; }
            break;
        }
        case 0x21: {
            uint8_t lo = read(pc++); uint8_t hi = read(pc++);
            hl = (hi << 8) | lo; cycles = 12; break;
        }
        case 0x22: write(hl++, a); cycles = 8; break;
        case 0x23: hl++; cycles = 8; break;
        case 0x28: { int8_t off = (int8_t)read(pc++); if (f & 0x80) { pc += off; cycles = 12; } else { cycles = 8; } break; }
        case 0x2A: a = read(hl++); cycles = 8; break;
        case 0x31: sp = (read(pc+1) << 8) | read(pc); pc += 2; cycles = 12; break;
        case 0x32: write(hl--, a); cycles = 8; break;
        case 0x3A: a = read(hl--); cycles = 8; break;
        case 0x3C: a++; setFlags(a == 0, 0, (a & 0x0F) == 0, (f & 0x10)); cycles = 4; break;
        case 0x3E: a = read(pc++); cycles = 8; break;
        case 0x76: cycles = 4; break;
        case 0x77: write(hl, a); cycles = 8; break;
        case 0x80: { uint16_t res = a + b; setFlags((res & 0xFF) == 0, 0, 0, res > 0xFF); a = (uint8_t)res; cycles = 4; break; }
        case 0x81: { uint16_t res = a + c; setFlags((res & 0xFF) == 0, 0, 0, res > 0xFF); a = (uint8_t)res; cycles = 4; break; }
        case 0x87: { uint16_t res = a + a; setFlags((res & 0xFF) == 0, 0, 0, res > 0xFF); a = (uint8_t)res; cycles = 4; break; }
        case 0x90: { uint8_t res = a - b; setFlags(res == 0, 1, 0, a < b); a = res; cycles = 4; break; }
        case 0xA0: { a &= b; setFlags(a == 0, 0, 1, 0); cycles = 4; break; }
        case 0xA8: { a ^= b; setFlags(a == 0, 0, 0, 0); cycles = 4; break; }
        case 0xB0: { a |= b; setFlags(a == 0, 0, 0, 0); cycles = 4; break; }
        case 0xAF: { a = 0; setFlags(1, 0, 0, 0); cycles = 4; break; }
        case 0xC1: { c = read(sp++); b = read(sp++); cycles = 12; break; }
        case 0xC5: { write(--sp, b); write(--sp, c); cycles = 16; break; }
        case 0xC9: { uint8_t lo = read(sp++); uint8_t hi = read(sp++); pc = (hi << 8) | lo; cycles = 16; break; }
        case 0xCD: {
            uint8_t lo = read(pc++); uint8_t hi = read(pc++);
            write(--sp, (pc >> 8) & 0xFF); write(--sp, pc & 0xFF);
            pc = (hi << 8) | lo; cycles = 24; break;
        }
        case 0xE0: { uint8_t addr = read(pc++); write(0xFF00 + addr, a); cycles = 12; break; }
        case 0xE1: { l = read(sp++); h = read(sp++); cycles = 12; break; }
        case 0xE5: { write(--sp, h); write(--sp, l); cycles = 16; break; }
        case 0xEA: { uint16_t addr = read(pc) | (read(pc+1) << 8); pc += 2; write(addr, a); cycles = 16; break; }
        case 0xF0: { uint8_t addr = read(pc++); a = read(0xFF00 + addr); cycles = 12; break; }
        case 0xC3: pc = (read(pc+1) << 8) | read(pc); cycles = 16; break;
        case 0xFA: { uint16_t addr = read(pc) | (read(pc+1) << 8); pc += 2; a = read(addr); cycles = 16; break; }
        case 0xFE: { uint8_t val = read(pc++); setFlags(a == val, 1, (a & 0x0F) < (val & 0x0F), a < val); cycles = 8; break; }
        case 0xF3: IME = false; cycles = 4; break;
        case 0xFB: IME = true;  cycles = 4; break;
        case 0xCB: cycles = handlePrefixCB(); break;
        default: 
            std::cout << "Unknown Opcode: 0x" << std::hex << (int)op 
                      << " at PC: 0x" << currentPC << std::endl;
            cycles = 4; 
            break;
    }
    
    handleInterrupts();
    return cycles;
}