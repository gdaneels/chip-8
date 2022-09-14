#include <inttypes.h>
#include <stdio.h>
#include "read.h"

void read_program(const char* program) {
	printf("reading CHIP-8 program with file name: %s\n", program);

	unsigned char buffer[10];
	FILE *ptr;

	ptr = fopen("/home/gdaneels/Downloads/ibmlogo.ch8", "rb");
	fread(buffer, sizeof(buffer), 1, ptr);
	for (size_t i = 0; i < 10; i++) {
		printf("%x", buffer[i]);
	}
	printf("\n");
}
