#include "stack.h"

#include <vector/vector.h>
#include <stdlib.h>
#include <utils/alloc.h>

struct stack *stack_push(struct stack *s, struct vector *vec)
{
    struct stack *res = malloc(sizeof(struct stack));
    if (res)
    {
        res->vec = vec;
        res->next = s;
        return res;
    }
    else
    {
        return s;
    }
}

struct stack *stack_pop(struct stack *s)
{
    if (!s)
    {
        return NULL;
    }
    else
    {
        struct stack *res = s->next;
        vector_apply_on_elts(s->vec, &free);
        vector_destroy(s->vec);
        free(s);
        return res;
    }
}

struct vector *stack_peek(struct stack *s)
{
    return s->vec;
}
