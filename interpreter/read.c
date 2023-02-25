#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "read.h"
#include "log.h"

static bool read(unsigned char* buffer, size_t buffer_size, FILE* fp, size_t* ret_code) {
	*ret_code = fread(buffer, sizeof(unsigned char), buffer_size, fp);
	if (ferror(fp)) {
        return false;
	}
    LOGD("Read program successfully.");
    return true;
}


bool read_program(const char* program, uint8_t memory[], uint16_t copy_to) {
	LOGD("Reading CHIP-8 program with file name: %s", program);
	
	size_t buffer_size = 4096;
	unsigned char buffer[buffer_size];
	FILE *fp;

    // file pointer
	fp = fopen(program, "rb");
	if (fp == NULL) {
		LOGD("Can not open file %s", program);
		return false;
	}
   
    size_t read_bytes = 0;
    bool success = read(buffer, buffer_size, fp, &read_bytes);
    if (success) {
        for (size_t i = 0; i < read_bytes; i++) {
            LOGV("%02x ", buffer[i]);
        }
        
        memcpy(&memory[copy_to], buffer, read_bytes);
        LOGD("Copied %zu bytes from read file to memory at location 0x%x.", read_bytes, copy_to);
    }

	// close the file
	fclose(fp);
    return success;
}
