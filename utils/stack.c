#include "stack.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>

#include "log.h"

typedef struct StackEntry StackEntry;

struct StackEntry
{
    StackType value;
    StackEntry* next;
    StackEntry* previous;
};

// this will be used as stack
static StackEntry* stack_ptr = NULL;

static void print_entry(StackEntry* sp)
{
    LOGD("StackEntry with value %u", sp->value);
    if (sp->previous) {
        LOGD("Has previous ptr with value %u", sp->previous->value);
    } else {
        LOGD("Does not have previous ptr.");
    }
    if (sp->next) {
        LOGD("Has next ptr with value %u", sp->next->value);
    } else {
        LOGD("Does not have next ptr.");
    }
}

static StackEntry* push_to(StackType value, StackEntry* sp)
{
    StackEntry* new_sp = malloc(sizeof(StackEntry));
    if (!new_sp) {
        return NULL;
    }
    new_sp->previous = NULL;
    if (sp) {
        sp->next = new_sp;
        new_sp->previous = sp;
    }
    new_sp->value = value;
    print_entry(new_sp);
    return new_sp;
}

bool stack_push(StackType value)
{
    StackEntry* tmp_sp = push_to(value, stack_ptr);
    if (!tmp_sp) {
        LOGE("Unable to push new stack entry.");
        return false;
    }
    // set new stack pointer to new stack entry
    stack_ptr = tmp_sp;
    return true;
}

bool stack_pop(StackType* top)
{
    if (!stack_ptr) {
        LOGD("No element on stack to pop...");
        return false;
    }
    // get the value
    *top = stack_ptr->value;
    // get the previous entry on the stack
    StackEntry* new_stack_ptr = stack_ptr->previous;
    // free the top stack entry
    free(stack_ptr);
    // assign the previous as the new current stack ptr
    stack_ptr = new_stack_ptr;
    return true;
}

bool stack_top(StackType* top)
{
    if (!stack_ptr) {
        LOGD("Stack is empty.");
        return false;
    }
    *top = stack_ptr->value;
    return true;
}

void stack_init(void)
{
    LOGD("Initialized stack.");
}
