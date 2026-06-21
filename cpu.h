#ifndef CPU_H
#define CPU_H
#include <cstdint>
class Memory;
class GameBoyCPU {
public:
    union { struct { uint8_t f, a; }; uint16_t af; };
    union { struct { uint8_t c, b; }; uint16_t bc; };
    union { struct { uint8_t e, d; }; uint16_t de; };
    union { struct { uint8_t l, h; }; uint16_t hl; };
    uint16_t pc, sp;
    GameBoyCPU(Memory* mem);
    int step();
    void debug();
private:
    Memory* memory;
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
};
#endif