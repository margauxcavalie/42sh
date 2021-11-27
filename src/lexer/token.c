#include "token.h"

#include <err.h>
#include <stdlib.h>
#include <utils/alloc.h>

struct token *token_new_word(char *word, size_t len)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_WORD;
    new->data.word = zalloc(sizeof(char) * (len + 1));
    strncpy(new->data.word, word, len);
    return new;
}

struct token *token_new_rw(enum rw_type type)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_RW;
    new->data.rw_type = type;
    return new;
}

struct token *token_new_op(enum op_type type)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_OP;
    new->data.op_type = type;
    return new;
}

struct token *token_new_eof(void)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_EOF;
    return new;
}

void token_free(struct token *token)
{
    if (token->type == TOKEN_WORD)
        free(token->data.word);
    free(token);
}
