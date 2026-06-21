#include <SDL2/SDL.h>
#include <iostream>
#include "cpu.h"
#include "memory.h"
#include "ppu.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("GameBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 4, 144 * 4, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetLogicalSize(renderer, 160, 144);

    if (!window || !renderer) {
        std::cerr << "Window/Renderer creation failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    Memory memory;
    GameBoyCPU cpu(&memory);
    PPU ppu(renderer);
    
    memory.setPPU(&ppu); 

    uint8_t program[] = {
        0x21, 0x00, 0x80,
        0x06, 0xFF,
        0x3E, 0x01,
        0x77,
        0x23,
        0x05,
        0x20, 0xFB,
        0x76
    };
    
    for(int i = 0; i < sizeof(program); i++) {
        memory.write(0x0100 + i, program[i]);
    }

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }

        int cyclesThisFrame = 0;
        while (cyclesThisFrame < 69905) {
            cyclesThisFrame += cpu.step();
        }

        ppu.update(cyclesThisFrame, renderer);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}