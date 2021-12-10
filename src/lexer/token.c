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

struct token *token_new_assignement_word(char *word, size_t len)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_ASSIGNEMENT_WORD;
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

struct token *token_new_ionumber(int number)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_IONUMBER;
    new->data.io_number = number;
    return new;
}

struct token *token_new_op(struct op_data op_data)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_OP;
    new->data.op_data = op_data;
    return new;
}

struct token *token_new_eof(void)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = TOKEN_EOF;
    return new;
}

bool is_op(struct token *tok, enum op_type op_type)
{
    if (tok->type != TOKEN_OP)
        return false;
    if (tok->data.op_data.type == op_type)
        return true;
    return false;
}

bool is_ionumber(struct token *tok)
{
    if (tok->type != TOKEN_IONUMBER)
        return false;
    return true;
}

bool is_rw(struct token *tok, enum rw_type rw_type)
{
    if (tok->type != TOKEN_RW)
        return false;
    if (tok->data.rw_type == rw_type)
        return true;
    return false;
}

bool is_word(struct token *tok)
{
    if (tok->type != TOKEN_WORD && tok->type != TOKEN_ASSIGNEMENT_WORD)
        return false;
    return true;
}

bool is_assignement_word(struct token *tok)
{
    if (tok->type != TOKEN_ASSIGNEMENT_WORD)
        return false;
    return true;
}

bool is_fname(struct token *tok)
{
    if (tok->type != TOKEN_FNAME)
        return false;
    return true;
}

void token_free(struct token *token)
{
    if (token->type == TOKEN_WORD || token->type == TOKEN_FNAME
        || token->type == TOKEN_ASSIGNEMENT_WORD)
        free(token->data.word);
    free(token);
}
