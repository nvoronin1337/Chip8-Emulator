// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdlib.h>
#include <stdexcept>
#include "chip8.h"
#include "sdl2_platform.h"

// DEBUGGING 
#define DEBUG 1
#if DEBUG 
#define TEST_PROGRAM "Airplane.ch8"
#include <iostream>
#define deb(x) std::cout << x << "\n"
#endif

const char* TITLE = "Chip8 Emulator";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::invalid_argument("Please specify a program to execute!");
        return EXIT_FAILURE;
    }
    else {
        int video_scale = 25;
        SDL_Platform platform(TITLE, VIDEO_WIDTH * video_scale, VIDEO_HEIGHT * video_scale, VIDEO_WIDTH, VIDEO_HEIGHT);

        Chip8 chip8;
        chip8.initialize();
        chip8.loadProgram(argv[1]);

        int videoPitch = sizeof(chip8.gfx[0]) * VIDEO_WIDTH;
        bool quit = false;

        while (!quit) {
            quit = platform.ProcessInput(chip8.keypad);

            chip8.emulateCycle();
            platform.Update(chip8.gfx, videoPitch);

        }
        
    }
    return EXIT_SUCCESS;
}
