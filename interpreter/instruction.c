#include "instruction.h"
#include <stdlib.h>
#include <assert.h>

#include "log.h"
#include "sdl.h"
#include "stack.h"

#define FLAG_REGISTER 15

// #define first_nibble(instr) (((instr) >> 12) & 0x000F)
// #define second_nibble(instr) (((instr) >> 8) & 0x000F)
// #define third_nibble(instr) (((instr) >> 4) & 0x000F)
// #define fourth_nibble(instr) ((instr)&0x000F)

#define BIT(byte, x) (((byte) >> (x)) & 0x1)

static inline uint8_t first_nibble(uint16_t instruction) {
    return (instruction >> 12) & 0x000F;
}

static inline uint8_t second_nibble(uint16_t instruction) {
    return (instruction >> 8) & 0x000F;
}

static inline uint8_t third_nibble(uint16_t instruction) {
    return (instruction >> 4) & 0x000F;
}

static inline uint8_t fourth_nibble(uint16_t instruction) {
    return instruction & 0x000F;
}

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
    ctx->pc = memory_location;
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
    uint8_t reg = second_nibble(instruction);
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
    uint8_t reg = second_nibble(instruction);
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
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
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
    uint8_t reg = second_nibble(instruction);
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
    uint8_t reg = second_nibble(instruction);
    // get NN value
    uint8_t value = read_nn(instruction);
    LOGD("Adding V%u register to value %u.", reg, value);
    ctx->v[reg] += value;
    print_v(ctx);
}

static void instruction_8XY0(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY0 (0x%x): VX register is set to value of VY.", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    ctx->v[reg_x] = ctx->v[reg_y];
    // TODO test this instruction
}

static void instruction_8XY1(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY1 (0x%x): VX register is set to bitwise OR of VX and VY", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    ctx->v[reg_x] |= ctx->v[reg_y];
    // TODO test this instruction
}

static void instruction_8XY2(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY2 (0x%x): VX register is set to bitwise AND of VX and VY", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    ctx->v[reg_x] &= ctx->v[reg_y];
    // TODO test this instruction
}

static void instruction_8XY3(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY3 (0x%x): VX register is set to bitwise XOR of VX and VY", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    ctx->v[reg_x] ^= ctx->v[reg_y];
    // TODO test this instruction
}

static void instruction_8XY4(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY4 (0x%x): VX register is set to sum of VX and VY", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    uint8_t orig_vx = ctx->v[reg_x];
    ctx->v[reg_x] += ctx->v[reg_y];
    if (orig_vx > ctx->v[reg_x]) {
        // overflow happened
        ctx->v[FLAG_REGISTER] = 1;
    } else {
        ctx->v[FLAG_REGISTER] = 0;
    }
    // TODO test this instruction
}

static void instruction_8XY5(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY5 (0x%x): VX register is set to VX - VY", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    ctx->v[reg_x] -= ctx->v[reg_y];
    if (ctx->v[reg_x] > ctx->v[reg_y]) {
        // set carry flag following chip 8 rules
        ctx->v[FLAG_REGISTER] = 1;
    } else {
        ctx->v[FLAG_REGISTER] = 0;
    }
    // TODO test this instruction
}

static void instruction_8XY7(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY7 (0x%x): VX register is set to VY - VX", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    ctx->v[reg_x] = ctx->v[reg_y] - ctx->v[reg_x];
    if (ctx->v[reg_y] > ctx->v[reg_x]) {
        // set carry flag following chip 8 rules
        ctx->v[FLAG_REGISTER] = 1;
    } else {
        ctx->v[FLAG_REGISTER] = 0;
    }
    // TODO test this instruction
}

static void instruction_8XY6(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XY6 (0x%x): shift right", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    // implement instruction
    // TODO test this instruction
}

static void instruction_8XYE(InterpreterContext* ctx, uint16_t instruction) {
    LOGD("Executing 8XYE (0x%x): shift left", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
    // implement instruction
    // TODO test this instruction
}

static void instruction_9XY0(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing 9XY0 (0x%x): skip one instruction if values VX and VY are "
         "different.", instruction);
    // get register x
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);
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
    uint8_t reg_x = second_nibble(instruction);
    // get register y
    uint8_t reg_y = third_nibble(instruction);

    // get x and y coordinate
    // module the width & height of the screen to wrap the starting position of the sprite
    uint16_t x_origin = ctx->v[reg_x] % IMAGE_WIDTH;
    uint16_t y_origin = ctx->v[reg_y] % IMAGE_HEIGHT;

    LOGD("reg_x (%u) and reg_y (%u) give origin at %u and %u.", reg_x, reg_y, x_origin, y_origin);

    ctx->v[0xF] = 0;

    // get the N rows
    uint8_t N = fourth_nibble(instruction);

    LOGD("Base memory address from register I: 0x%x (%u).", ctx->i, ctx->i);
    for(uint8_t row = 0; row < N; row++) {
        // row per row, add to the base memory address saved in register I
        uint16_t sprite_row_loc = ctx->i + row;
        uint8_t data_byte = ctx->memory[sprite_row_loc];
        LOGD("Drawing from memory address: 0x%x (%u) with value %u.", sprite_row_loc, sprite_row_loc, data_byte);

        // the scaling should dissappear here in my opinion, let the graphics lib handle this
        uint16_t y = (y_origin + row) * IMAGE_SCALE_FACTOR;
        for (uint8_t column = 0; column < 8; column++) {
            uint16_t x = (x_origin + column) * IMAGE_SCALE_FACTOR;
            uint8_t screen_pixel = sdl_instr_get_pixel(ctx->image, x, y);
            /* 
             * do 7 - column, as bit 0 is most right bit, while bit 7 is most left bit
             * left most pixel in sprite byte is at bit 7
             * right most pixel in sprite byte is at bit 0
            */
            uint8_t bit = 7 - column;
            uint8_t sprite_pixel = BIT(data_byte, bit); // TODO get pixel from sprite in memory

            LOGD("XOR'ing pixel (%u, %u): screen pixel is %u, sprite pixel is %u.", x, y, screen_pixel, sprite_pixel);

            if (sprite_pixel) {
                if (!screen_pixel) {
                    // draw
                    sdl_instr_set_pixel(ctx->image, x, y);
                } else {
                    // undraw pixel
                    sdl_instr_unset_pixel(ctx->image, x, y);
                    ctx->v[0xF] = 1;
                }
            }
            // reach edge of screen? stop
            if (x == IMAGE_WIDTH - 1) {
                break;
            }
        }
        // reach end of screen? stop
        if (y == IMAGE_HEIGHT - 1) {
            break;
        }
    }
}

static void instruction_FX29(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing FX29: font character instruction.");
    uint8_t reg_x = second_nibble(instruction);
    // character should be stored in 4 first bits of VX register
    uint8_t character = 0x000F & ctx->v[reg_x];
    // a character is 5 bytes wide
    ctx->i = ADDR_BUILTIN_FONT + (5 * character); 
    LOGD("I register set to address (0x%x, %u) of character 0x%x.", ctx->i, ctx->i, character);
}

static void instruction_FX55(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing FX55 (0x%x): store memory.", instruction);
    uint8_t x = second_nibble(instruction); 

    assert(x <= MAX_V_REG);

    for (uint8_t v_reg_i = 0; v_reg_i <= x; v_reg_i++) {
        uint16_t addr = ctx->i + v_reg_i;
        ctx->memory[addr] = ctx->v[v_reg_i];
        LOGD("Storing variable register V%u (value %u) to memory address (0x%x, %u).", v_reg_i, ctx->v[v_reg_i], addr, addr);
    }
}

static void instruction_FX65(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing FX65 (0x%x): load memory.", instruction);
    uint8_t x = second_nibble(instruction); 

    assert(x <= MAX_V_REG);

    for (uint8_t v_reg_i = 0; v_reg_i <= x; v_reg_i++) {
        uint16_t addr = ctx->i + v_reg_i;
        ctx->v[v_reg_i] = ctx->memory[addr];
        LOGD("Loading value (%u) at memory (0x%x, %u) in variable register V%u.", ctx->memory[v_reg_i], addr, addr, v_reg_i);
    }
}

static void instruction_FX07(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing FX07 (0x%x): sets VX to current value of delay timer.", instruction);
    uint8_t reg_x = second_nibble(instruction); 
    ctx->v[reg_x] = ctx->delay_timer;
}

static void instruction_FX15(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing FX15 (0x%x): sets delay timer to value in VX.", instruction);
    uint8_t reg_x = second_nibble(instruction); 
    ctx->delay_timer = ctx->v[reg_x];
}

static void instruction_FX18(InterpreterContext* ctx, uint16_t instruction)
{
    LOGD("Executing FX18 (0x%x): sets sound timer to value in VX.", instruction);
    uint8_t reg_x = second_nibble(instruction); 
    ctx->sound_timer = ctx->v[reg_x];
}

instruction_cb instruction_get_F(uint16_t instruction, OPCODE* op_code) {
    assert(first_nibble(instruction) == 0xF);
    uint8_t last_byte = read_nn(instruction); 
    switch(last_byte) {
        case 0x29:
            return instruction_FX29;
            break;
        case 0x55:
            return instruction_FX55;
            break;
        case 0x65:
            return instruction_FX65;
            break;
        case 0x07:
            return instruction_FX07;
            break;
        case 0x15:
            return instruction_FX15;
            break;
        case 0x18:
            return instruction_FX18;
            break;
        default:
            break;
    }
    return NULL;
}

instruction_cb instruction_get_8(uint16_t instruction, OPCODE* op_code) {
    assert(first_nibble(instruction) == 0x8);
    uint8_t last_byte = read_nn(instruction); 
    switch(last_byte) {
        case 0x0:
            return instruction_8XY0;
            break;
        case 0x1:
            return instruction_8XY1;
            break;
        case 0x2:
            return instruction_8XY2;
            break;
        case 0x3:
            return instruction_8XY3;
            break;
        case 0x4:
            return instruction_8XY4;
            break;
        case 0x5:
            return instruction_8XY4;
            break;
        case 0x6:
            return instruction_8XY6;
            break;
        case 0x7:
            return instruction_8XY7;
            break;
        case 0xE:
            return instruction_8XYE;
            break;
        default:
            break;
    }
    return NULL;
}

void test_DXYN(InterpreterContext* ctx) {
    // X = 0 (V0), Y = 1 (V1), N = 5 (5 pixels tall font)
    uint16_t instruction_f = 0xD015;
    // set VX (V0) and VY (V1) registers
    ctx->v[0] = 0; // V0 register, x = 0
    ctx->v[1] = 0; // V1 register, y = 0
    // set I register to first font character F
    ctx->i = ADDR_BUILTIN_FONT + 5 * 15;
    instruction_DXYN(ctx, instruction_f);
    LOGD("Executing second cmd.");
    // X = 2 (V2), Y = 3 (V3), N = 5 (5 pixels tall font)
    uint16_t instruction_a = 0xD235;
    // set VX (V2) and VY (V3) registers
    ctx->v[2] = 10; // V2 register, x = 10
    ctx->v[3] = 10; // V3 register, y = 10
    // set I register to first font character A
    ctx->i = ADDR_BUILTIN_FONT + 5 * 10;
    instruction_DXYN(ctx, instruction_a);
}

void test_FX55(InterpreterContext* ctx) {
    ctx->v[0] = 20; // V2 register, x = 10
    ctx->v[1] = 30; // V3 register, y = 10
    ctx->v[2] = 4; // V2 register, x = 10
    ctx->v[3] = 10; // V3 register, y = 10
    uint16_t instruction_f = 0xF255;
    instruction_FX55(ctx, instruction_f);
}

void test_FX65(InterpreterContext* ctx) {
    ctx->v[0] = 20; // V2 register, x = 10
    ctx->v[1] = 30; // V3 register, y = 10
    ctx->v[2] = 4; // V2 register, x = 10
    ctx->v[3] = 10; // V3 register, y = 10
    print_v(ctx);
    uint16_t instruction_f = 0xF255;
    instruction_FX55(ctx, instruction_f);
    ctx->v[0] = 1; // V2 register, x = 10
    ctx->v[1] = 2; // V3 register, y = 10
    ctx->v[2] = 3; // V2 register, x = 10
    ctx->v[3] = 4; // V3 register, y = 10
    uint16_t instruction_f2 = 0xF165;
    instruction_FX65(ctx, instruction_f2);
    print_v(ctx);
}

void instruction_test(InterpreterContext* ctx, OPCODE op_code) {
    switch(op_code) {
        case OPCODE_DXYN:
            LOGD("Testing instruction DXYN.");
            test_DXYN(ctx);
            break;
        case OPCODE_FX55:
            LOGD("Testing instruction FX55.");
            test_FX55(ctx);
            break;
        case OPCODE_FX65:
            LOGD("Testing instruction FX65.");
            test_FX65(ctx);
            break;
        default:
            break;
    }
}

instruction_cb instruction_get(uint16_t instruction, OPCODE* op_code)
{
    uint8_t val_first_nibble = first_nibble(instruction);
    uint8_t val_second_nibble = second_nibble(instruction);
    uint8_t val_third_nibble = third_nibble(instruction);
    uint8_t val_fourth_nibble = fourth_nibble(instruction);
    LOGD(
        "Instruction: 0x%x. Nibbles: %x, %x, %x and %x.",
        instruction,
        val_first_nibble,
        val_second_nibble,
        val_third_nibble,
        val_fourth_nibble);

    switch (val_first_nibble) {
    case 0:
        switch (val_second_nibble) {
        case 0:
            switch (val_fourth_nibble) {
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
