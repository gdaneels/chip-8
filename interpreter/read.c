#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "read.h"

void read_program(const char* program, uint8_t memory[], uint16_t copy_to) {
	printf("Reading CHIP-8 program with file name: %s\n", program);
	
	size_t BUFFER_SIZE = 4096;
	unsigned char buffer[BUFFER_SIZE];
	FILE *fp;

	fp = fopen(program, "rb"); // file pointer
	if (fp == NULL) {
		fprintf(stderr, "Can not open file %s\n", program);
		return;
	}

	size_t ret_code = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, fp);
	if (ferror(fp)) {
		fprintf(stderr, "Error reading file.\n");
	} else {
		printf("Reading successful.\n");
	}

	for (size_t i = 0; i < ret_code; i++) {
		printf("%02x ", buffer[i]);
	}
	printf("Read %zu bytes.\n", ret_code);
	
	memcpy(&memory[copy_to], buffer, ret_code);
	printf("Copied %zu bytes from read file to memory at location 0x%x.\n", ret_code, copy_to);
	printf("\n");
	// close the file
	fclose(fp);
}
