#include "runtime.h"

#include <parser/function/ast_function.h>
#include <stdio.h>
#include <utils/alloc.h>

struct runtime *runtime_init()
{
    struct runtime *new = xmalloc(sizeof(struct runtime));
    new->last_status = 0;
    new->functions = hash_map_func_init(32);
    new->stack_function = NULL;
    new->loops_to_break = 0;
    new->loops_count = 0;
    new->encountered_continue = false;
    new->variables = var_hash_map_init();
    new->encountered_exit = false;
    new->encountered_break = false;
    new->no_new_status = false;
    return new;
}

void runtime_free(struct runtime *rt)
{
    hash_map_func_free(rt->functions, &ast_node_free);
    var_hash_map_free(rt->variables);
    free(rt);
}

void runtime_set_status(struct runtime *rt, int status)
{
    // update $?
    char *key = zalloc(sizeof(char) * 2);
    char *tmp = "?";
    char *value = zalloc(sizeof(char) * 12);
    sprintf(value, "%d", status);
    // printf("%s",value);
    strcpy(key, tmp);
    var_hash_map_insert(rt->variables, key, value);
    // update rt->last_status
    rt->last_status = status;
}
