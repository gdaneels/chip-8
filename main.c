#include <stdio.h>
#include <stdint.h> 
#include <string.h>
#include <inttypes.h>

#include "stack.h"
#include "interpreter.h"

int main(int argc, char* argv[]) {
	//init_builtin_font();
	//print_memory_val(0x50);
//	stack_push(5);
//	StackType top;
//	stack_top(&top);
//	printf("top of stack is %" PRIu16 "\n", top);
//	stack_push(6);
//	if (stack_top(&top)) {
//		printf("top of stack is %" PRIu16 "\n", top);
//		stack_pop(&top);
//		printf("come here;\n");
//		stack_pop(&top); // why does this trigger a double free?
//		stack_top(&top);
//		printf("top of stack is now %" PRIu16 "\n", top);
//	}

	if (argc <= 1) {
		printf("No input program given. Exiting.\n");
		return 1;
	}
	const char* program = argv[1];
	init(program);
	// run();
	return 0;
}
