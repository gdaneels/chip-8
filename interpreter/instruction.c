#include "instruction.h"

#include <stdlib.h>
#include <assert.h>

#include "log.h"
#include "sdl.h"
#include "stack.h"

#define FIRST_NIBBLE(instr) (((instr) >> 12) & 0x000F)
#define SECOND_NIBBLE(instr) (((instr) >> 8) & 0x000F)
#define THIRD_NIBBLE(instr) (((instr) >> 4) & 0x000F)
#define FOURTH_NIBBLE(instr) ((instr)&0x000F)

static inline uint8_t read_nn(uint16_t instr)
{
    return instr & 0x00FF;
}

static inline uint16_t read_nnn(uint16_t instr)
{
    return instr & 0x0FFF;
}

static void print_v(InterpreterContext* ctx)
{
    LOGD("*** General-purpose variable register contents: ***");
    for (uint8_t i = 0; i < sizeof(ctx->v); i++) {
        LOGD("*** v[%u] = 0x%x (%u)", i, ctx->v[i], ctx->v[i]);
    }
}

static void print_i(InterpreterContext* ctx)
{
    LOGD("*** Index register = 0x%x (%u) ***", ctx->i, ctx->i);
}

static void instruction_0NNN(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Instruction 0NNN not implemented.");
    exit(EXIT_FAILURE);
}

static void instruction_1NNN(InterpreterContext* ctx, uint16_t instruction)
{
    // for now assume 1NNN (jump to NNN) instruction
    LOGD("Executing 1NNN: jump instruction.");
    uint16_t memory_location = read_nnn(instruction);
    LOGD(
        "Setting PC from 0x%x (%u) to 0x%x (%u).",
        ctx->pc,
        ctx->pc,
        memory_location,
        memory_location);
    // TODO enable again
    // pc = memory_location;
    LOGD("Executing 1NNN");
}

static void instruction_2NNN(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 2NNN: call subroutine instruction.");
    uint16_t memory_location = read_nnn(instruction);
    LOGD("Pushing current PC 0x%x (%u) to stack.", ctx->pc, ctx->pc);
    if (!stack_push(ctx->pc)) {
        exit(EXIT_FAILURE);
    }
    LOGD(
        "Setting PC from 0x%x (%u) to 0x%x (%u).",
        ctx->pc,
        ctx->pc,
        memory_location,
        memory_location);
    ctx->pc = memory_location;
}

static void instruction_3XNN(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 3XNN: skip one instruction if value VX equals NN.");
    // get register
    uint8_t reg = SECOND_NIBBLE(instruction);
    // get NN value
    uint8_t value = read_nn(instruction);
    // if register and value are equal skip instruction
    if (ctx->v[reg] == value) {
        LOGD(
            "Skipping next instruction because V%u (%u) == value %u.",
            reg,
            ctx->v[reg],
            value);
        ctx->pc += 2;
    } else {
        LOGD(
            "Do nothing because V%u (%u) != value %u.",
            reg,
            ctx->v[reg],
            value);
    }
}

static void instruction_4XNN(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD(
        "Executing 4XNN: skip one instruction if value VX is different from NN.");
    // get register
    uint8_t reg = SECOND_NIBBLE(instruction);
    // get NN value
    uint8_t value = read_nn(instruction);
    // if register and value are equal skip instruction
    if (ctx->v[reg] != value) {
        LOGD(
            "Skipping next instruction because V%u (%u) != value %u.",
            reg,
            ctx->v[reg],
            value);
        ctx->pc += 2;
    } else {
        LOGD(
            "Do nothing because V%u (%u) == value %u.",
            reg,
            ctx->v[reg],
            value);
    }
}

static void instruction_5XY0(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 5XY0: skip one instruction if values VX and VY are equal.");
    // get register x
    uint8_t reg_x = SECOND_NIBBLE(instruction);
    // get register y
    uint8_t reg_y = THIRD_NIBBLE(instruction);
    // if register and value are equal skip instruction
    if (ctx->v[reg_x] == ctx->v[reg_y]) {
        LOGD(
            "Skipping next instruction because V%u (%u) == value V%u (%u).",
            reg_x,
            ctx->v[reg_x],
            reg_y,
            ctx->v[reg_y]);
        ctx->pc += 2;
    } else {
        LOGD(
            "Do nothing because V%u (%u) != value V%u (%u).",
            reg_x,
            ctx->v[reg_x],
            reg_y,
            ctx->v[reg_y]);
    }
}

static void instruction_6XNN(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 6XNN: set VX register to NN value instruction.");
    print_v(ctx);
    // get register
    uint8_t reg = SECOND_NIBBLE(instruction);
    // get NN value
    uint8_t value = read_nn(instruction);
    LOGD("Setting V%u register to value %u.", reg, value);
    ctx->v[reg] = value;
    print_v(ctx);
}

static void instruction_7XNN(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 7XNN: add the value NN to VX register.");
    print_v(ctx);
    // get register
    uint8_t reg = SECOND_NIBBLE(instruction);
    // get NN value
    uint8_t value = read_nn(instruction);
    LOGD("Adding V%u register to value %u.", reg, value);
    ctx->v[reg] += value;
    print_v(ctx);
}

static void instruction_9XY0(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 9XY0: skip one instruction if values VX and VY are "
         "different.");
    // get register x
    uint8_t reg_x = SECOND_NIBBLE(instruction);
    // get register y
    uint8_t reg_y = THIRD_NIBBLE(instruction);
    // if register and value are equal skip instruction
    if (ctx->v[reg_x] != ctx->v[reg_y]) {
        LOGD(
            "Skipping next instruction because V%u (%u) != value V%u (%u).",
            reg_x,
            ctx->v[reg_x],
            reg_y,
            ctx->v[reg_y]);
        ctx->pc += 2;
    } else {
        LOGD(
            "Do nothing because V%u (%u) == value V%u (%u).",
            reg_x,
            ctx->v[reg_x],
            reg_y,
            ctx->v[reg_y]);
    }
}

static void instruction_ANNN(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing ANNN: set index register I to value NNN.");
    uint16_t value = read_nnn(instruction);
    ctx->i = value;
    LOGD("Setting index register to value NNN (0x%x, %u).", value, value);
    print_i(ctx);
}

static void instruction_00E0(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 00E0: clear screen instruction.");
    sdl_instr_clear_screen(ctx->image);
}

static void instruction_00EE(InterpreterContext* ctx, uint16_t instruction)
{
    uint16_t return_pc = 0;
    LOGD("Executing 00EE: return from subroutine instruction.");
    if (!stack_pop(&return_pc)) {
        LOGE("Popping from stack failed.");
        exit(EXIT_FAILURE);
    }
    LOGD("Popped PC 0x%x (%u) from stack.", return_pc, return_pc);
    ctx->pc = return_pc;
}

static void instruction_DXYN(InterpreterContext* ctx, uint16_t instruction)
{
    // get register x
    uint8_t reg_x = SECOND_NIBBLE(instruction);
    // get register y
    uint8_t reg_y = THIRD_NIBBLE(instruction);

    // get x and y coordinate
    // module the width & height of the screen to wrap the starting position of the sprite
    uint8_t x_coord = ctx->v[reg_x] % IMAGE_WIDTH;
    uint8_t y_coord = ctx->v[reg_y] % IMAGE_HEIGHT;

    ctx->v[0xF] = 0;

    // get the N rows
    uint8_t N = FOURTH_NIBBLE(instruction);

    LOGD("Base memory address from register I: 0x%x (%u).", ctx->i, ctx->i);
    for(uint8_t n = 0; n < N; n++) {
        // row per row, add to the base memory address saved in register I
        uint16_t row_sprite_data = ctx->i + n;
        LOGD("Drawing from memory address: 0x%x (%u).", row_sprite_data, row_sprite_data);
        for (uint8_t i_x = 0; i_x < 8; i_x++) {
            for (uint8_t i_y = 0; i_y < 8; i_y++) {
                LOGD("XOR'ing pixel at: 0x%x (%u).", row_sprite_data, row_sprite_data);

                uint8_t screen_pixel = sdl_instr_get_pixel(ctx->image, x_coord, y_coord);
                uint8_t sprite_pixel = 0; // TODO get pixel from sprite in memory

                if (sprite_pixel) {
                    if (!screen_pixel) {
                        // draw
                        sdl_instr_set_pixel(ctx->image, x_coord, y_coord);
                    } else {
                        // undraw pixel
                        sdl_instr_unset_pixel(ctx->image, x_coord, y_coord);
                        ctx->v[0xF] = 1;
                    }
                }
                // TODO check if you reach end of screen: if so, stop *row*
            }
            // TODO check if you reach bottom of screen: if so, stop
        }
    }

}

static void instruction_FX29(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing FX29: font character instruction.");
    uint8_t reg_x = SECOND_NIBBLE(instruction);
    // character should be stored in 4 first bits of VX register
    uint8_t character = 0x000F & ctx->v[reg_x];
    // a character is 5 bytes wide
    ctx->i = ADDR_BUILTIN_FONT + (5 * character); 
    LOGD("I register set to address (0x%x, %u) of character 0x%x.", ctx->i, ctx->i, character);
}

instruction_cb instruction_get_F(uint16_t instruction, OPCODE* op_code) {
    assert(FIRST_NIBBLE(instruction) == 0xF);
    uint8_t last_byte = read_nn(instruction); 
    switch(last_byte) {
        case 0x29:
            return instruction_FX29;
            break;
        default:
            break;
    }
    return NULL;
}

instruction_cb instruction_get(uint16_t instruction, OPCODE* op_code)
{
    uint8_t first_nibble = FIRST_NIBBLE(instruction);
    uint8_t second_nibble = SECOND_NIBBLE(instruction);
    uint8_t third_nibble = THIRD_NIBBLE(instruction);
    uint8_t fourth_nibble = FOURTH_NIBBLE(instruction);
    LOGD(
        "Instruction: 0x%x. Nibbles: %x, %x, %x and %x.",
        instruction,
        first_nibble,
        second_nibble,
        third_nibble,
        fourth_nibble);

    switch (first_nibble) {
    case 0:
        switch (second_nibble) {
        case 0:
            switch (fourth_nibble) {
            case 0:
                return instruction_00E0;
                break;
            case 0xe:
                return instruction_00EE;
                break;
            default:
                break;
            }
            break;
        default:
            return instruction_0NNN;
            break;
        }
        break;
    case 1:
        return instruction_1NNN;
        break;
    case 2:
        return instruction_2NNN;
        break;
    case 3:
        return instruction_3XNN;
        break;
    case 4:
        return instruction_4XNN;
        break;
    case 5:
        return instruction_5XY0;
        break;
    case 6:
        return instruction_6XNN;
        break;
    case 7:
        return instruction_7XNN;
        break;
    case 9:
        return instruction_9XY0;
        break;
    case 0xA:
        return instruction_ANNN;
        break;
    case 0xD:
        return instruction_DXYN;
        break;
    case 0xF:
        // redirect to other function to avoid too much nesting
        return instruction_get_F(instruction, op_code);
        break;
    case 0xe:;
        // uint16_t test1 = 2;
        //  printf("come in e");
        break;
    default:
        break;  // this has to be removed and replaced by the default statement.
    }
    // TODO set opcode correctly
    *op_code = OPCODE_00E0;

    return NULL;
}
