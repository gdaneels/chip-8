#ifndef INTERPRETER_H
#define INTERPRETER_H

#define IMAGE_SCALE_FACTOR 4
#define IMAGE_WIDTH IMAGE_SCALE_FACTOR * 64
#define IMAGE_HEIGHT IMAGE_SCALE_FACTOR * 32
// shouldn't it be lowercase x in 0x50 and 0x200?
#define ADDR_BUILTIN_FONT 0X50 // memory address should go from 0x50 to 0x9F
#define ADDR_START_PROGRAM 0X200 // start memory address of chip-8 program

void init(const char* program);
void run(void);

#endif 
