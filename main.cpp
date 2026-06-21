#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "input.h"

void loadROM(const char* filename, Memory& mem) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file!" << std::endl;
        return;
    }
    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), {});
    mem.loadCartridge(buffer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("GameBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 160 * 4, 144 * 4, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 160, 144);

    Memory memory;
    GameBoyCPU cpu(&memory);
    PPU ppu(renderer);
    Input input;

    memory.setPPU(&ppu);
    memory.setInput(&input);

    if (argc > 1) {
        loadROM(argv[1], memory);
    }

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            input.handleEvent(e);
        }

        int cyclesThisFrame = 0;
        while (cyclesThisFrame < 69905) {
            cyclesThisFrame += cpu.step();
        }

        ppu.update(cyclesThisFrame, renderer, memory);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}