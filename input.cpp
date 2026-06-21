#include "input.h"

Input::Input() : buttons(0xFF) {}

void Input::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
        bool down = (e.type == SDL_KEYDOWN);
        switch (e.key.keysym.sym) {
            case SDLK_z:      down ? buttons &= ~(1 << 0) : buttons |= (1 << 0); break;
            case SDLK_x:      down ? buttons &= ~(1 << 1) : buttons |= (1 << 1); break;
            case SDLK_RETURN: down ? buttons &= ~(1 << 3) : buttons |= (1 << 3); break;
            case SDLK_s:      down ? buttons &= ~(1 << 2) : buttons |= (1 << 2); break;
            case SDLK_RIGHT:  down ? buttons &= ~(1 << 4) : buttons |= (1 << 4); break;
            case SDLK_LEFT:   down ? buttons &= ~(1 << 5) : buttons |= (1 << 5); break;
            case SDLK_UP:     down ? buttons &= ~(1 << 6) : buttons |= (1 << 6); break;
            case SDLK_DOWN:   down ? buttons &= ~(1 << 7) : buttons |= (1 << 7); break;
        }
    }
}

uint8_t Input::getJoypadState() { return buttons; }