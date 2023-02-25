#ifndef READ_H
#define READ_H

#include <stdbool.h>
#include <inttypes.h>

bool read_program(const char* program, uint8_t memory[], uint16_t copy_to);

#endif 
