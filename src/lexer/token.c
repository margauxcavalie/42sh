#include "token.h"

#include <err.h>
#include <stdlib.h>
#include <utils/alloc.h>

struct token *token_new(enum token_type type)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = type;
    return new;
}

void token_free(struct token *token)
{
    if (token->type == TOKEN_WORD)
        free(token->word);
    free(token);
}
