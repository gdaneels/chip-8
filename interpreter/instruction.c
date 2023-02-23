#include <stdlib.h>
#include "instruction.h"
#include "log.h"
#include "stack.h"
#include "sdl.h"

#define FIRST_NIBBLE(instr) (((instr) >> 12) & 0x000F)
#define SECOND_NIBBLE(instr) (((instr) >> 8) & 0x000F)
#define THIRD_NIBBLE(instr) (((instr) >> 4) & 0x000F)
#define FOURTH_NIBBLE(instr) ((instr) & 0x000F)

static inline uint8_t read_nn(uint16_t instr) {
    return instr & 0x00FF;
}

static inline uint16_t read_nnn(uint16_t instr) {
    return instr & 0x0FFF;
}

static void instruction_0NNN(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Instruction 0NNN not implemented.");
    exit(EXIT_FAILURE);
}

static void instruction_1NNN(InterpreterContext* ctx, uint16_t instruction) {
    // for now assume 1NNN (jump to NNN) instruction
    LOGD("Executing 1NNN: jump instruction.");
    uint16_t memory_location = read_nnn(instruction);
    LOGD("Setting PC from 0x%x (%u) to 0x%x (%u).", ctx->pc, ctx->pc, memory_location, memory_location);
    // TODO enable again
    // pc = memory_location;
    LOGD("Executing 1NNN");
}

static void instruction_2NNN(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 2NNN: call subroutine instruction.");
    uint16_t memory_location = read_nnn(instruction);
    LOGD("Pushing current PC 0x%x (%u) to stack.", ctx->pc, ctx->pc);
    if (!stack_push(ctx->pc)) {
        exit(EXIT_FAILURE);
    }
    LOGD("Setting PC from 0x%x (%u) to 0x%x (%u).", ctx->pc, ctx->pc, memory_location, memory_location);
    ctx->pc = memory_location;
}

static void instruction_6XNN(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 6XNN: set VX register to NN value instruction.");
    // get register
    uint8_t reg = SECOND_NIBBLE(instruction);
    // get NN value
    uint8_t value = read_nn(instruction);
    LOGD("Setting V%u register to value %u.", reg, value);
    ctx->v[reg] = value;
}

static void instruction_00E0(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 00E0: clear screen instruction.");
    sdl_instr_clear_screen(ctx->image);
}

static void instruction_00EE(InterpreterContext* ctx, uint16_t instruction) {
    uint16_t return_pc = 0;
    LOGD("Executing 00EE: return from subroutine instruction.");
    if (!stack_pop(&return_pc)) {
        LOGE("Popping from stack failed.");
        exit(EXIT_FAILURE);
    }
    LOGD("Popped PC 0x%x (%u) from stack.", return_pc, return_pc);
    ctx->pc = return_pc;
}

instruction_cb instruction_get(uint16_t instruction, OPCODE* op_code) {
    uint8_t first_nibble = FIRST_NIBBLE(instruction);
    uint8_t second_nibble = SECOND_NIBBLE(instruction);
    uint8_t third_nibble = THIRD_NIBBLE(instruction);
    uint8_t fourth_nibble = FOURTH_NIBBLE(instruction);
    LOGD("Instruction: 0x%x. Nibbles: %x, %x, %x and %x.", instruction, first_nibble, second_nibble, third_nibble, fourth_nibble);

    switch (first_nibble) {
        case 0:
            {
                switch (second_nibble) {
                    case 0:
                        {
                            switch (fourth_nibble) {
                                case 0:
                                    {
                                        return instruction_00E0;
                                        break;
                                    }
                                case 0xe:
                                    {
                                        return instruction_00EE;

                                        break;
                                    }
                                default:
                                    {
                                        break;
                                    }
                            }

                            break;
                        }
                    default:
                        {
                            return instruction_0NNN;
                            break;
                        }
                }
                break;
            }
        case 1:
            {
                return instruction_1NNN;
                break;
            }
        case 2:
            {
                return instruction_2NNN;
                break;
            }
        case 6:
            {
                return instruction_6XNN;
                break;
            }
        case 0xe:;
           uint16_t test1 = 2;
            // printf("come in e");
            break;
        default:
            break; // this has to be removed and replaced by the default statement.
    }
    // TODO set opcode correctly
    *op_code = OPCODE_00E0;

    return NULL;
}
