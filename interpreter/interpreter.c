#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include <string.h>

#include "instruction.h"
#include "log.h"
#include "read.h"
#include "sdl.h"
#include "stack.h"

InterpreterContext interpreter_context = (InterpreterContext){ 0 };

static void init_builtin_font(void)
{
    const uint8_t font_memory[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF1, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80   // F
    };
    // copy the font data into our memory
    memcpy(
        &interpreter_context.memory[ADDR_BUILTIN_FONT],
        font_memory,
        sizeof(font_memory));
}

static void print_memory_val(uint16_t addr)
{
    // TODO also print hexadecimal value
    LOGI(
        "Value at memory addr %" PRIu16 " is: %u",
        addr,
        interpreter_context.memory[addr]);
}

static void add_instruction(void)
{
    uint16_t instruction = 0x1234;
    memcpy(
        &interpreter_context.memory[ADDR_START_PROGRAM],
        &instruction,
        sizeof(instruction));
    LOGI("Added instruction to memory: %x.", instruction);
}

void init(const char* program, bool read_only)
{
    // read in program
    // initialize stack
    stack_init();
    // initialize read only option with user input
    interpreter_context.read_only = read_only;
    // initialize instruction count to 0
    interpreter_context.instruction_count = 0;
    // initialize program counter
    interpreter_context.pc = ADDR_START_PROGRAM;
    // initialize graphics window
    if (!interpreter_context.read_only) {
        interpreter_context.image = sdl_init(IMAGE_WIDTH, IMAGE_HEIGHT);
    }
    // CHANGE TO 0
    interpreter_context.delay_timer = 60;
    interpreter_context.sound_timer = 60;

    // initialize built in font
    init_builtin_font();

    // read user program
    if (!read_program(
            program,
            interpreter_context.memory,
            ADDR_START_PROGRAM)) {
        LOGE("Failed to read CHIP-8 program.");
        exit(EXIT_FAILURE);
    }
    
    if (read_only) {
        LOGD("Read-only mode activated.");
    }
}
static uint16_t fetch(InterpreterContext* ctx) {
        return (ctx->memory[ctx->pc] << 8) | ctx->memory[ctx->pc + 1];
}

static void increment_pc(InterpreterContext* ctx) {
        ctx->instruction_count++;
        LOGD("***********************************");
        uint16_t next_pc = ctx->pc + 2;
        LOGD(
            "Current PC = %u (0x%x) (instruction #%u), incrementing to %u (0x%x).",
            ctx->pc,
            ctx->pc,
            ctx->instruction_count,
            next_pc,
            next_pc);
        // increment the PC immediately by 2
        ctx->pc = next_pc;
}

static instruction_cb decode(uint16_t instruction, OPCODE* opcode) {
        // decode
        instruction_cb instruction_function = instruction_get(instruction, opcode);
        if (instruction_function == NULL) {
            // could not identify instruction, exit
            LOGE(
                "Unable to fetch instruction callback for instruction %u (%x)",
                instruction, instruction);
            exit(EXIT_FAILURE);
        }
        return instruction_function;
}

static void update_timers(InterpreterContext* ctx) {
    if (ctx->delay_timer > 0) {
        ctx->delay_timer--;
        LOGD("Decremented delay timer to %u.", ctx->delay_timer);
    }

    if (ctx->sound_timer > 0) {
        ctx->sound_timer--;
        LOGD("Decremented sound timer to %u.", ctx->sound_timer);
    }
}

void run(void)
{
    uint16_t instruction_count = 0;
    uint16_t instruction = 0x0;
    instruction_cb instruction_function = NULL;
    OPCODE opcode;
    // for now, we ignore the timers as they are not clear for me yet

    if (!interpreter_context.read_only) {
        sdl_prepare_scene(interpreter_context.image);
    }
    //while (instruction_count < 2) {
    //while (interpreter_context.instruction_count < 60) {
    while(1) {
        if (!interpreter_context.read_only) {
            sdl_get_input(interpreter_context.image);
        }

        uint32_t start_time = sdl_get_ticks_ms();

        for (size_t i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
            instruction = fetch(&interpreter_context);
            increment_pc(&interpreter_context);
            instruction_function = decode(instruction, &opcode);

            if (interpreter_context.read_only) {
                continue;
            }
            
            // execute
            LOGD("Executing instruction with opcode %u.", opcode);
            instruction_function(&interpreter_context, instruction);
            // instruction_test(&interpreter_context, OPCODE_FX65);
        }

        // render screen
        sdl_present_scene(interpreter_context.image);

        update_timers(&interpreter_context);

        uint32_t stop_time = sdl_get_ticks_ms();
        uint32_t execution_time = (stop_time - start_time);
        LOGD("execution_time was = %"PRIu32" ms (%"PRIu32" - %"PRIu32").", execution_time, start_time, stop_time);
        // 1000 ms over the number of frames, gives the x ms per frame
        uint32_t fps_ms = (1000.0 / FRAMES_PER_SECOND);
        uint32_t delay = fps_ms - execution_time;
        LOGD("delay = %"PRIu32" and fps_ms = %"PRIu32".", delay, fps_ms);
        if (delay > fps_ms) {
            // underflow happening
            // meaning that all time was spent by executed instructions
            // so you should continue immediately
            continue;
        } 
        sdl_do_delay(delay);
    }
    sdl_free(interpreter_context.image);
}
