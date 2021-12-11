#pragma once
#include <hash_map_function/hash_map_function.h>
#include <stdbool.h>
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
    int loops_to_break;
    int loops_count;
    bool encountered_continue;
    bool encountered_exit;
    bool encountered_break;
};

struct runtime *runtime_init();
void runtime_free(struct runtime *rt);
void runtime_set_status(struct runtime *rt, int status);
