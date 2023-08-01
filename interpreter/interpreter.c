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
    // initialize program counter
    interpreter_context.pc = ADDR_START_PROGRAM;
    // initialize graphics window
    if (!interpreter_context.read_only) {
        interpreter_context.image = sdl_init(IMAGE_WIDTH, IMAGE_HEIGHT);
    }
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

void run(void)
{
    uint16_t instruction_count = 0;
    uint16_t instruction = 0x0;
    instruction_cb instruction_function = NULL;
    // for now, we ignore the timers as they are not clear for me yet

    if (!interpreter_context.read_only) {
        sdl_prepare_scene(interpreter_context.image);
    }
    //while (instruction_count < 2) {
    while(1) {
        if (!interpreter_context.read_only) {
            sdl_get_input();
        }

        // fetch
        instruction = (interpreter_context.memory[interpreter_context.pc] << 8)
            | interpreter_context.memory[interpreter_context.pc + 1];
        instruction_count++;
        LOGD("***********************************");
        uint16_t next_pc = interpreter_context.pc + 2;
        LOGD(
            "Current PC = %u (0x%x) (instruction #%u), incrementing to %u (0x%x).",
            interpreter_context.pc,
            interpreter_context.pc,
            instruction_count,
            next_pc,
            next_pc);
        // increment the PC immediately by 2
        interpreter_context.pc = next_pc;

        // decode
        OPCODE opcode;
        instruction_function = instruction_get(instruction, &opcode);
        if (instruction_function == NULL) {
            // could not identify instruction, exit
            LOGE(
                "Unable to fetch instruction callback for instruction %u (%x)",
                instruction, instruction);
            exit(EXIT_FAILURE);
        }

        if (interpreter_context.read_only) {
            LOGD("%u", opcode);
            continue;
        }

        // execute
        LOGD("Executing instruction with opcode %u.", opcode);
        instruction_function(&interpreter_context, instruction);
        // instruction_test(&interpreter_context, OPCODE_FX65);

        sdl_present_scene(interpreter_context.image);
        // should be replaced by timer of chip 8?
        sdl_do_delay(200);
    }
    sdl_free(interpreter_context.image);
}
