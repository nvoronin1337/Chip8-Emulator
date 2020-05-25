// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdlib.h>
#include <stdexcept>
#include "chip8.h"

// DEBUGGING 
#define DEBUG 1
#if DEBUG 
#define TEST_PROGRAM "Airplane.ch8"
#include <iostream>
#define deb(x) std::cout << x << "\n"
#endif

int main(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::invalid_argument("Please specify a program to execute!");
        return EXIT_FAILURE;
    }
    else {
        Chip8 chip8;
        chip8.initialize();
        chip8.loadProgram(argv[1]);

        while (true) {
            chip8.emulateCycle();
            if (chip8.getDrawFlag()) {
                chip8.drawGraphics();
            }

            chip8.setKeys();
        }
        
    }
    return EXIT_SUCCESS;
}
