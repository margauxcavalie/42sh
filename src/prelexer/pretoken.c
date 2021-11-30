#include "pretoken.h"

#include <err.h>
#include <stdlib.h>
#include <utils/alloc.h>

struct pretoken *pretoken_new(enum pretoken_type type, const char *str,
                              size_t len)
{
    struct pretoken *new = zalloc(sizeof(struct pretoken));
    new->type = type;
    if (str != NULL && type != PRETOKEN_EOF && type != PRETOKEN_ERROR)
    {
        new->str = zalloc(sizeof(char) * (len + 1));
        strncpy(new->str, str, len);
    }
    else
    {
        new->str = NULL;
    }
    return new;
}

void pretoken_free(struct pretoken *pretoken)
{
    if (pretoken->type != PRETOKEN_EOF && pretoken->type != PRETOKEN_EOF)
        free(pretoken->str);
    free(pretoken);
}
