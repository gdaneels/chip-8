#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "read.h"
#include "interpreter.h"
#include "sdl.h"

#define FIRST_NIBBLE(instr) (((instr) >> 12) & 0x000F)
#define SECOND_NIBBLE(instr) (((instr) >> 8) & 0x000F)
#define THIRD_NIBBLE(instr) (((instr) >> 4) & 0x000F)
#define FOURTH_NIBBLE(instr) ((instr) & 0x000F)

static uint8_t memory[4096];
static uint16_t pc; // program counter, points at the current instruction in memory
static uint16_t i; // index register, to point at locations in memory
static uint8_t delay_timer; // decremented at a rate of 60 Hz until it reaches 0
static uint8_t sound_timer; // gives off a beeping sound as long as it's not 0, 
		     // decremented at a rate of 60 Hz until it reaches 0
static uint8_t v[8]; // general-purpose variable registers numbered 0 through F hexadecimal (ie, 0 through 15 in decimal),
		     // called V0 through VF
		     // VF is also called the flag register

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
	memcpy(&memory[ADDR_BUILTIN_FONT], font_memory, sizeof(font_memory));
}

static void print_memory_val(uint16_t addr) {
	// TODO also print hexadecimal value
	printf("Value at memory addr %" PRIu16 " is: %u\n", addr, memory[addr]);
}

static void add_instruction(void) {
	uint16_t instruction = 0x1234;
	memcpy(&memory[ADDR_START_PROGRAM], &instruction, sizeof(instruction));
	printf("Added instruction to memory: %x.\n", instruction);
}

static uint16_t read_jump_addr(uint16_t instr) {
    return instr & 0x0FFF;
}

void init(const char* program) {
    // read in program
	read_program(program, memory, ADDR_START_PROGRAM);
    // initialize program counter
	pc = 0;
	printf("instruction at %u is %02x\n", ADDR_START_PROGRAM, memory[ADDR_START_PROGRAM + 131]);
}

void run(void) {
    uint16_t instruction_count = 0;
    // for now, we ignore the timers as they are not clear for me yet
    pc = ADDR_START_PROGRAM;	

    Image* image = sdl_init(IMAGE_WIDTH, IMAGE_HEIGHT);

    sdl_prepare_scene(image);
    sdl_instr_draw_pixel(image, 50, 50);
    while(1) {
        sdl_get_input();

        // fetch
        // TO TEST THIS INSTRUCTION BUILDING
        uint16_t instruction = (memory[pc+1] << 8) | memory[pc];
        instruction_count++;
        printf("***********************************\n");
        printf("PC = %u (instruction #%u).\n", pc, instruction_count);
        // increment the PC immediately by 2
        pc += 2;

        // decode
        uint8_t first_nibble = FIRST_NIBBLE(instruction);
        uint8_t second_nibble = SECOND_NIBBLE(instruction);
        uint8_t third_nibble = THIRD_NIBBLE(instruction);
        uint8_t fourth_nibble = FOURTH_NIBBLE(instruction);
        printf("Instruction: 0x%x. Nibbles: %x, %x, %x and %x.\n", instruction, first_nibble, second_nibble, third_nibble, fourth_nibble);
        // execute
        switch (first_nibble) {
            case 0:
                // printf("Clear the screen!\n");
                printf("Executing 00E0: clear screen instruction.\n");
                // probably this leads to 00E0 aka `clear the screen`
                sdl_instr_clear_screen(image);
                break;
            case 1:
                // for now assume 1NNN (jump to NNN) instruction
                printf("Executing 1NNN: jump instruction.\n");
                uint16_t new_pc = read_jump_addr(instruction);
                printf("Setting PC from 0x%x (%u) to 0x%x (%u).\n", pc, pc, new_pc, new_pc);
                pc = new_pc;

                break;
            case 0xe:
                // printf("come in e\n");
                break;
            default:
                break; // this has to be removed and replaced by the default statement.
        }
        sdl_present_scene(image);
        // should be replaced by timer of chip 8?
		sdl_do_delay(16);
    }
    sdl_free(image);
}
