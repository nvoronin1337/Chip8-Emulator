#include "chip8.h"

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;

const int SPRAY_LENGTH = 8;

const char* FILE_MODE = "rb";

const int FILE_ERROR = 1;
const int MEMORY_ERROR = 2;
const int READING_ERROR = 3;

const uint8_t chip8_fontset[FONTSET] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::Table0() {
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8() {
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE() {
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF() {
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::setupOpcodeTables() {
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::exec1NNN;
    table[0x2] = &Chip8::exec2NNN;
    table[0x3] = &Chip8::exec3XNN;
    table[0x4] = &Chip8::exec4XNN;
    table[0x5] = &Chip8::exec5XY0;
    table[0x6] = &Chip8::exec6XNN;
    table[0x7] = &Chip8::exec7XNN;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::exec9XY0;
    table[0xA] = &Chip8::execANNN;
    table[0xB] = &Chip8::execBNNN;
    table[0xC] = &Chip8::execCXNN;
    table[0xD] = &Chip8::execDXYN;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    table0[0x0] = &Chip8::exec00E0;
    table0[0xE] = &Chip8::exec00EE;

    table8[0x0] = &Chip8::exec8XY0;
    table8[0x1] = &Chip8::exec8XY1;
    table8[0x2] = &Chip8::exec8XY2;
    table8[0x3] = &Chip8::exec8XY3;
    table8[0x4] = &Chip8::exec8XY4;
    table8[0x5] = &Chip8::exec8XY5;
    table8[0x6] = &Chip8::exec8XY6;
    table8[0x7] = &Chip8::exec8XY7;
    table8[0xE] = &Chip8::exec8XYE;

    tableE[0xE] = &Chip8::execEX9E;
    tableE[0x1] = &Chip8::execEXA1;

    tableF[0x07] = &Chip8::execFX07;
    tableF[0x0A] = &Chip8::execFX0A;
    tableF[0x15] = &Chip8::execFX15;
    tableF[0x18] = &Chip8::execFX18;
    tableF[0x1E] = &Chip8::execFX1E;
    tableF[0x29] = &Chip8::execFX29;
    tableF[0x33] = &Chip8::execFX33;
    tableF[0x55] = &Chip8::execFX55;
    tableF[0x65] = &Chip8::execFX65;
}

void Chip8::initialize() {
    pc = START_ADDRESS;
    opcode = 0;
    I = 0;
    sp = 0;
    
    // Clear display
    // Clear stack
    // Clear registers V0-VF
    // Clear memory
    memset(gfx, 0, sizeof(gfx));
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    memset(memory, 0, sizeof(memory));

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    for (unsigned int i = 0; i < FONTSET; i++) {
        memory[FONTSET_START_ADDRESS + i] = chip8_fontset[i];
    }

    assert(FONTSET_START_ADDRESS + sizeof chip8_fontset < START_ADDRESS);
    setupOpcodeTables();
}

void Chip8::loadProgram(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; i++) {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
}

void Chip8::emulateCycle() {
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];  

    pc += 2;

    // Decode and Execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // Update timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        if (sound_timer == 1) {
            printf("BEEP!\n");
            --sound_timer;
        }
        else {
            printf("NO BEEP!\n");
        }
    }
}

void Chip8::exec00E0() {
    memset(gfx, 0, sizeof gfx);
    drawFlag = 1;
}

void Chip8::exec00EE() {
    --sp;
    pc = stack[sp];
}

void Chip8::exec1NNN() {
    pc = opcode & 0x0FFFu;
}

void Chip8::exec2NNN() {
    stack[sp] = pc;
    ++sp;
    pc = opcode & 0x0FFFu;
}

void Chip8::exec3XNN() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t NN = opcode & 0x00FFu;
    if (V[X] == NN) {
        pc += 2;
    }
}

void Chip8::exec4XNN() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t NN = opcode & 0x00FFu;
    if (V[X] != NN) {
        pc += 2;
    }
}

void Chip8::exec5XY0() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x0F0) >> 4u;

    if (V[X] == V[Y]) {
        pc += 2;
    }
}

void Chip8::exec6XNN() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t NN = (opcode & 0x00FF);

    V[X] = NN;
}

void Chip8::exec7XNN() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t NN = (opcode & 0x00FF);

    V[X] += NN;
}

void Chip8::exec8XY0() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;

    V[X] = V[Y];
}

void Chip8::exec8XY1() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;

    V[X] |= V[Y];
}

void Chip8::exec8XY2() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;

    V[X] &= V[Y];
}

void Chip8::exec8XY3() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;

    V[X] ^= V[Y];
}

void Chip8::exec8XY4() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;

    uint16_t sum = V[X] + V[Y];
    if (sum > 255u) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }

    V[X] = sum & 0xFFu;
}

void Chip8::exec8XY5() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;
    
    if (V[X] > V[Y]) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }

    V[X] -= V[Y];
}

void Chip8::exec8XY6() {
    uint8_t X = (opcode & 0x0F00) >> 8u;

    V[0xF] = (V[X] & 0x1u);
    V[X] >>= 1;
}

void Chip8::exec8XY7() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;

    if (V[Y] > V[X]) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }
}

void Chip8::exec8XYE() {
    uint8_t X = (opcode & 0x0F00) >> 8u;

    V[0xF] = (V[X] & 0x1u);
    V[X] << 1;
}

void Chip8::exec9XY0() {
    uint8_t X = (opcode & 0x0F00) >> 8u;
    uint8_t Y = (opcode & 0x00F0) >> 4u;

    if (V[X] != V[Y]) {
        pc += 2;
    }
}
    
void Chip8::execANNN() {
    uint16_t NNN = opcode & 0x0FFFu;
    I = NNN;
}

void Chip8::execBNNN() {
    uint16_t NNN = opcode & 0x0FFFu;
    pc = NNN + V[0];
}

void Chip8::execCXNN() {
    srand(time(0));
    uint8_t rand_byte = rand() % 255 + 1;
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t NN = opcode & 0x00FFu;
    V[X] = NN & rand_byte;
}

void Chip8::execDXYN() {
    // draws sprite
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t Y = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;
    uint8_t pixel;

    V[0xF] = 0;
    for (int row = 0; row < height; row++) {
        pixel = memory[I + row];
        for (int col = 0; col < SPRAY_LENGTH; col++) {
            if ((pixel & (0x80 >> col)) != 0) {
                if (gfx[X + col + ((Y + row) * 64)] == 1) 
                    V[0xF] = 0;
                gfx[X + col + ((Y + row) * 64)] ^= 1;
            }
        }
    }

    drawFlag = true;
}

void Chip8::execEX9E() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t key = V[X];
    if (keypad[key]) {
        pc += 2;
    }
}

void Chip8::execEXA1() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t key = V[X];
    if (!keypad[key]) {
        pc += 2;
    }
}

void Chip8::execFX07() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    V[X] = delay_timer;
}

void Chip8::execFX0A() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    int key_num = 0;
    for (uint8_t key : keypad) {
        if (key) {
            V[X] = key_num;
            return;
        }
        key_num++;
    }
    pc -= 2;
}

void Chip8::execFX15() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    delay_timer = V[X];
}

void Chip8::execFX18() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    sound_timer = V[X];
}

void Chip8::execFX1E() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint16_t sum = V[X] + I;
    if (sum > 0xFFF) {
        V[0xF] = 1;
    }
    else {
        V[0xF] = 0;
    }

    I += V[X];
}

void Chip8::execFX29() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    I = FONTSET_START_ADDRESS + (5 * V[X]);
}

void Chip8::execFX33() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    uint8_t value = V[X];
    // Ones-place
    memory[I + 2] = value % 10;
    value /= 10;

    // Tens-place
    memory[I + 1] = value % 10;
    value /= 10;

    // Hundreds-place
    memory[I] = value % 10;
}

void Chip8::execFX55() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= X; ++i) {
        memory[I + i] = V[i];
    }
}

void Chip8::execFX65() {
    uint8_t X = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= X; ++i) {
        V[i] = memory[I + i];
    }
}

void Chip8::setKeys() {

}

void Chip8::setupGraphics() {

}

void Chip8::setupInput() {

}

void Chip8::drawGraphics() {
    drawFlag = 0;
}

bool Chip8::getDrawFlag() const {
    return drawFlag;
}
