#include "runtime.h"

#include <parser/function/ast_function.h>
#include <utils/alloc.h>

struct runtime *runtime_init()
{
    struct runtime *new = xmalloc(sizeof(struct runtime));
    new->last_status = 0;
    new->functions = hash_map_func_init(32);
    new->loops_to_break = 0;
    new->loops_count = 0;
    new->encountered_continue = false;
    new->variables = var_hash_map_init();
    new->encountered_exit = false;
    return new;
}

void runtime_free(struct runtime *rt)
{
    hash_map_free(rt->variables, free);
    hash_map_func_free(rt->functions, &ast_function_free);
    free(rt);
}

void runtime_set_status(struct runtime *rt, int status)
{
    rt->last_status = status;
}
