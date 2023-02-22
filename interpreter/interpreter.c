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
static uint8_t v[16]; // general-purpose variable registers numbered 0 through F hexadecimal (ie, 0 through 15 in decimal),
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
	LOGI("Value at memory addr %" PRIu16 " is: %u", addr, memory[addr]);
}

static void add_instruction(void) {
	uint16_t instruction = 0x1234;
	memcpy(&memory[ADDR_START_PROGRAM], &instruction, sizeof(instruction));
	LOGI("Added instruction to memory: %x.", instruction);
}

static inline uint8_t read_nn(uint16_t instr) {
    return instr & 0x00FF;
}

static inline uint16_t read_nnn(uint16_t instr) {
    return instr & 0x0FFF;
}

void init(const char* program) {
    // read in program
	read_program(program, memory, ADDR_START_PROGRAM);
    // initialize stack
    stack_init();
    // initialize program counter
	pc = ADDR_START_PROGRAM;
}

void run(void) {
    uint16_t instruction_count = 0;
    uint16_t instruction = 0x0;
    // for now, we ignore the timers as they are not clear for me yet

    Image* image = sdl_init(IMAGE_WIDTH, IMAGE_HEIGHT);

    sdl_prepare_scene(image);
    sdl_instr_draw_pixel(image, 50, 50);
    while(instruction_count < 1) {
        sdl_get_input();

        // fetch
        instruction = (memory[pc] << 8) | memory[pc+1];
        instruction_count++;
        LOGD("***********************************");
        uint16_t next_pc = pc + 2;
        LOGD("Current PC = %u (0x%x) (instruction #%u), incrementing to %u (0x%x).", pc, pc, instruction_count, next_pc, next_pc);
        // increment the PC immediately by 2
        pc = next_pc;

        // decode
        uint8_t first_nibble = FIRST_NIBBLE(instruction);
        uint8_t second_nibble = SECOND_NIBBLE(instruction);
        uint8_t third_nibble = THIRD_NIBBLE(instruction);
        uint8_t fourth_nibble = FOURTH_NIBBLE(instruction);
        LOGD("Instruction: 0x%x. Nibbles: %x, %x, %x and %x.", instruction, first_nibble, second_nibble, third_nibble, fourth_nibble);
        // execute
        switch (first_nibble) {
            case 0:
                {
                    switch (second_nibble) {
                        case 0:
                            {
                                switch (fourth_nibble) {
                                    case 0:
                                        {
                                            LOGD("Executing 00E0: clear screen instruction.");
                                            sdl_instr_clear_screen(image);
                                            break;
                                        }
                                    case 0xe:
                                        {
                                            uint16_t return_pc = 0;
                                            LOGD("Executing 00EE: return from subroutine instruction.");
                                            if (!stack_pop(&return_pc)) {
                                                LOGE("Popping from stack failed.");
                                                exit(1);
                                            }
                                            LOGD("Popped PC 0x%x (%u) from stack.", return_pc, return_pc);
                                            pc = return_pc;

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
                                LOGD("Instruction 0NNN not implemented.");
                                exit(1);
                                break;
                            }
                    }
                    break;
                }
            case 1:
                {
                    // for now assume 1NNN (jump to NNN) instruction
                    LOGD("Executing 1NNN: jump instruction.");
                    uint16_t memory_location = read_nnn(instruction);
                    LOGD("Setting PC from 0x%x (%u) to 0x%x (%u).", pc, pc, memory_location, memory_location);
                    // pc = memory_location;

                    break;
                }
            case 2:
                {
                    LOGD("Executing 2NNN: call subroutine instruction.");
                    uint16_t memory_location = read_nnn(instruction);
                    LOGD("Pushing current PC 0x%x (%u) to stack.", pc, pc);
                    if (!stack_push(pc)) {
                        exit(1);
                    }
                    LOGD("Setting PC from 0x%x (%u) to 0x%x (%u).", pc, pc, memory_location, memory_location);
                    pc = memory_location;
                    break;
                }
            case 6:
                {
                    LOGD("Executing 6XNN: set VX register to NN value instruction.");
                    // get register
                    uint8_t reg = second_nibble;
                    // get NN value
                    uint8_t value = read_nn(instruction);
                    LOGD("Setting V%u register to value %u.", reg, value);
                    v[reg] = value;
                    break;
                }
            case 0xe:
               uint16_t test1 = 2;
                // printf("come in e");
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
