#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <inttypes.h>
#include <string.h>

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
    init(program);
    run();
    return 0;
}
