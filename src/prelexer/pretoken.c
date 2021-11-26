#include "pretoken.h"

#include <err.h>
#include <stdlib.h>
#include <utils/alloc.h>

struct pretoken *pretoken_new(enum pretoken_type type)
{
    struct pretoken *new = zalloc(sizeof(struct pretoken));
    new->type = type;
    return new;
}

void pretoken_free(struct pretoken *pretoken)
{
    if (pretoken->type != PRETOKEN_EOF)
        free(pretoken->str);
    free(pretoken);
}
