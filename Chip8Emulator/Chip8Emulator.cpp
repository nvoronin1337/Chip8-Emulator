// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdlib.h>
#include <stdexcept>
#include "chip8.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::invalid_argument("Please specify a program to execute!");
        return EXIT_FAILURE;
    }
    else {
        Chip8 chip8;
        chip8.initialize();
        chip8.loadProgram(argv[1]);
    }
    return EXIT_SUCCESS;
}
