#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "log.h"
#include "read.h"
#include "interpreter.h"
#include "sdl.h"
#include "stack.h"
#include "instruction.h"

InterpreterContext interpreter_context = (InterpreterContext) { 0 };

static void init_builtin_font(void) {
	const uint8_t font_memory[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF1, 0x10, 0xF0, 0x80, 0xF0, // 2
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
	// copy the font data into our memory
	memcpy(&interpreter_context.memory[ADDR_BUILTIN_FONT], font_memory, sizeof(font_memory));
}

static void print_memory_val(uint16_t addr) {
	// TODO also print hexadecimal value
	LOGI("Value at memory addr %" PRIu16 " is: %u", addr, interpreter_context.memory[addr]);
}

static void add_instruction(void) {
	uint16_t instruction = 0x1234;
	memcpy(&interpreter_context.memory[ADDR_START_PROGRAM], &instruction, sizeof(instruction));
	LOGI("Added instruction to memory: %x.", instruction);
}

void init(const char* program) {
    // read in program
	if (!read_program(program, interpreter_context.memory, ADDR_START_PROGRAM)) {
        LOGE("Failed to read CHIP-8 program.");
        exit(EXIT_FAILURE);
    }
    // initialize stack
    stack_init();
    // initialize program counter
	interpreter_context.pc = ADDR_START_PROGRAM;
}

void run(void) {
    uint16_t instruction_count = 0;
    uint16_t instruction = 0x0;
    instruction_cb instruction_function = NULL;
    // for now, we ignore the timers as they are not clear for me yet

    interpreter_context.image = sdl_init(IMAGE_WIDTH, IMAGE_HEIGHT);

    sdl_prepare_scene(interpreter_context.image);
    sdl_instr_draw_pixel(interpreter_context.image, 50, 50);
    while(instruction_count < 2) {
    // while(1) {
        sdl_get_input();

        // fetch
        instruction = (interpreter_context.memory[interpreter_context.pc] << 8) | interpreter_context.memory[interpreter_context.pc+1];
        instruction_count++;
        LOGD("***********************************");
        uint16_t next_pc = interpreter_context.pc + 2;
        LOGD("Current PC = %u (0x%x) (instruction #%u), incrementing to %u (0x%x).", interpreter_context.pc, interpreter_context.pc, instruction_count, next_pc, next_pc);
        // increment the PC immediately by 2
        interpreter_context.pc = next_pc;

        // decode
        OPCODE opcode;
        instruction_function = instruction_get(instruction, &opcode);
        if (instruction_function == NULL) {
            // could not identify instruction, exit
            LOGE("Unable to fetch instruction callback for instruction %u.", instruction);
            exit(EXIT_FAILURE);
        }

        // execute
        LOGD("Executing instruction with opcode %u.", opcode);
        instruction_function(&interpreter_context, instruction);

        sdl_present_scene(interpreter_context.image);
        // should be replaced by timer of chip 8?
	    sdl_do_delay(16);
    }
    sdl_free(interpreter_context.image);
}
