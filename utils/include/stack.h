#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <inttypes.h>

typedef uint16_t StackType;

bool stack_push(StackType value);
bool stack_pop(StackType* value);
bool stack_top(StackType* value);
void stack_init(void);

#endif // STACK_H
