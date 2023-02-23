#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "sdl.h"

#define IMAGE_SCALE_FACTOR 4
#define IMAGE_WIDTH IMAGE_SCALE_FACTOR * 64
#define IMAGE_HEIGHT IMAGE_SCALE_FACTOR * 32
// shouldn't it be lowercase x in 0x50 and 0x200?
#define ADDR_BUILTIN_FONT 0X50 // memory address should go from 0x50 to 0x9F
#define ADDR_START_PROGRAM 0X200 // start memory address of chip-8 program

typedef struct {
    uint8_t memory[4096];
    uint16_t pc; // program counter, points at the current instruction in memory
    uint16_t i; // index register, to point at locations in memory
    uint8_t delay_timer; // decremented at a rate of 60 Hz until it reaches 0
    uint8_t sound_timer; // gives off a beeping sound as long as it's not 0, 
          // decremented at a rate of 60 Hz until it reaches 0
    uint8_t v[16]; // general-purpose variable registers numbered 0 through F hexadecimal (ie, 0 through 15 in decimal),
                 // called V0 through VF
                 // VF is also called the flag register
    Image* image;
} InterpreterContext;

void init(const char* program);
void run(void);

#endif 
