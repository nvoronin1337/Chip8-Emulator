#pragma once

#ifndef CHIP8
#define CHIP8

#include <fstream>
#include <unordered_map>
#include <cstdint>
#include <assert.h>

const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTERS = 16;  
const unsigned int RESOLUTION = 2048;
const unsigned int STACK_LEVELS = 16;
const unsigned int KEYS = 16;
const unsigned int FONTSET = 80;

/**
 * System memory map
 * 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 * 0x200-0xFFF - Program ROM and work RAM
*/

class Chip8 {
private:
    uint16_t opcode;
    uint8_t memory[MEMORY_SIZE]{};

    uint8_t V[REGISTERS]; // V0 up to VE

    uint16_t I;   // Index register
    uint16_t pc;  // program counter

    uint32_t gfx[RESOLUTION];

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t stack[STACK_LEVELS];
    uint8_t sp;

    uint8_t key[KEYS];

    bool drawFlag = false;

    //opcode functions
    void op_null() {};

    void exec00E0();
    void exec00EE();
    void exec1NNN();
    void exec2NNN();
    void exec3XNN();
    void exec4XNN() {};
    void exec5XY0() {};
    void exec6XNN() {};
    void exec7XNN() {};
    void exec8XY0() {};
    void exec8XY1() {};
    void exec8XY2() {};
    void exec8XY3() {};
    void exec8XY4() {};
    void exec8XY5() {};
    void exec8XY6() {};
    void exec8XY7() {};
    void exec8XYE() {};
    void exec9XY0() {};
    void execANNN() {};
    void execBNNN() {};
    void execCXNN() {};
    void execDXYN() {};
    void execEX9E() {};
    void execEXA1() {};
    void execFX07() {};
    void execFX0A() {};
    void execFX15() {};
    void execFX18() {};
    void execFX1E() {};
    void execFX29() {};
    void execFX33() {};
    void execFX55() {};
    void execFX65() {};

    void Table0();
    void Table8();
    void TableE();
    void TableF();

    typedef void (Chip8::* OpcodeFunction)();
    OpcodeFunction table[0xF + 1] = { &Chip8::op_null };
    OpcodeFunction table0[0xE + 1] = { &Chip8::op_null };
    OpcodeFunction table8[0xE + 1] = { &Chip8::op_null };
    OpcodeFunction tableE[0xE + 1] = { &Chip8::op_null };
    OpcodeFunction tableF[0x65 + 1] = { &Chip8::op_null };

public:
    void initialize();
    void setupOpcodeTables();
    void loadProgram(const char* filename);
    void emulateCycle();
    void setKeys();

    static void setupGraphics();
    static void setupInput();
    static void drawGraphics();
};
#endif
