#include "runtime.h"

#include <utils/alloc.h>

struct runtime *runtime_init()
{
    struct runtime *new = xmalloc(sizeof(struct runtime));
    new->last_status = 0;
    return new;
}

void runtime_free(struct runtime *rt)
{
    free(rt);
}

void runtime_set_status(struct runtime *rt, int status)
{
    rt->last_status = status;
}
