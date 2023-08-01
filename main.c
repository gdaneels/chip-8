#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

#include "interpreter.h"
#include "log.h"
#include "stack.h"

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        LOGE("No input program given. Exiting.");
        return 1;
    }

    const char* program = argv[1];
    bool read_only = false;
    if (argc == 3) {
        read_only = argv[2]; 
    }
    init(program, read_only);
    run();
    return 0;
}
