#pragma once
#include <hash_map_function/hash_map_function.h>
#include <stack_function/stack.h>
#include <stdbool.h>
#include <stdio.h>
#include <var_expansion/var_expansion.h>

/**
 *
 * The goal of this structure is to contain
 * all the global data needed during the runtime
 *
 */
struct runtime
{
    int last_status; // is the return of the last execution
    struct hash_map *variables;
    struct hash_map_function *functions;
    struct stack *stack_function;
    int loops_to_break;
    int loops_count;
    bool encountered_continue;
    bool encountered_exit;
    bool encountered_break;
    bool no_new_status;
    FILE *file; // if script
};

struct runtime *runtime_init();
void runtime_free(struct runtime *rt);
void runtime_set_status(struct runtime *rt, int status);
