#include <SDL2/SDL.h>
#include <iostream> // This fixes the red squiggly lines
#include "cpu.h"
#include "memory.h"
#include "ppu.h"

int main(int argc, char* argv[]) {
    // 1. Initialize SDL at the very beginning
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("GameBoy", 100, 100, 160, 144, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        std::cerr << "Window/Renderer creation failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 2. Setup Emulator Components
    Memory memory;
    GameBoyCPU cpu(&memory);
    PPU ppu(renderer);
    
    memory.setPPU(&ppu); 

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }

        int cycles = cpu.step();
        ppu.update(cycles, renderer);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }

    std::cout << "Emulator has successfully started!" << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}