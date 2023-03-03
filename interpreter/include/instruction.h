#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

#include "interpreter.h"

typedef enum
{
    OPCODE_0NNN,
    OPCODE_00E0,
    OPCODE_1NNN,
    OPCODE_00EE,
    OPCODE_2NNN,
    OPCODE_3XNN,
    OPCODE_4XNN,
    OPCODE_5XYN,
    OPCODE_9XY0,
    OPCODE_6XNN,
    OPCODE_7XNN,
    OPCODE_DXYN,
    OPCODE_NULL,
    OPCODE_MAX
} OPCODE;

typedef void (*instruction_cb)(InterpreterContext*, uint16_t);

instruction_cb instruction_get(uint16_t instruction, OPCODE* op_code);

void instruction_test(InterpreterContext* ctx, OPCODE op_code);

#endif  // INSTRUCTION_H
