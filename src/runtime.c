#include "runtime.h"

#include <utils/alloc.h>

struct runtime *runtime_init()
{
    struct runtime *new = xmalloc(sizeof(struct runtime));
    new->last_status = 0;
    new->variables = var_hash_map_init();
    return new;
}

void runtime_free(struct runtime *rt)
{
    hash_map_free(rt->variables, free);
    free(rt);
}

void runtime_set_status(struct runtime *rt, int status)
{
    rt->last_status = status;
}
