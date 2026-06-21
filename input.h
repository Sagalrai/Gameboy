#ifndef INPUT_H
#define INPUT_H
#include <SDL2/SDL.h>
#include <cstdint>

class Input {
public:
    Input();
    void handleEvent(SDL_Event& e);
    uint8_t getJoypadState();
private:
    uint8_t buttons;
};
#endif