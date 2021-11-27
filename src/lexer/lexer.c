#include "lexer.h"

#include <err.h>
#include <prelexer/prelexer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>

enum op_type match_op_type(struct pretoken *new_pretoken)
{
    // initalizes the lookup table
    struct matching_op lookup_table[] = { { ";", 1, OP_SEMICOLON },
                                          { "\n", 1, OP_LINEFEED } };
    size_t lt_size = sizeof(lookup_table) / sizeof(struct matching_op);
    size_t count = 0;
    while (count < lt_size)
    {
        // if we match the content of the lookup table
        if (strcmp(lookup_table[count].str, new_pretoken->str) == 0)
            return lookup_table[count].type;
        count += 1;
    }
    // if we do not match any of the possible operator
    return OP_UNKNOWN;
}

enum rw_type match_rw_type(struct pretoken *new_pretoken)
{
    // initalizes the lookup table
    struct matching_rw lookup_table[] = { { "if", 2, RW_IF },
                                          { "then", 4, RW_THEN },
                                          { "elif", 4, RW_ELIF },
                                          { "else", 4, RW_ELSE },
                                          { "fi", 2, RW_FI } };
    size_t lt_size = sizeof(lookup_table) / sizeof(struct matching_rw);
    size_t count = 0;
    while (count < lt_size)
    {
        // if we match the content of the lookup table
        if (strcmp(lookup_table[count].str, new_pretoken->str) == 0)
            return lookup_table[count].type;
        count += 1;
    }
    // if we do not match any of the possible reserved word
    return RW_UNKNOWN;
}

struct token *get_next_token(struct lexer *lexer)
{
    struct pretoken *new_pretoken =
        lexer->pretokens->data[lexer->pretoken_index];
    lexer->pretoken_index += 1;

    if (new_pretoken->type == PRETOKEN_EOF)
    {
        struct token *eof_token = token_new_eof();
        return eof_token;
    }
    else if (new_pretoken->type == PRETOKEN_OPERATOR)
    {
        lexer->line_index = 0;
        enum op_type op_type = match_op_type(new_pretoken);
        if (op_type == OP_UNKNOWN) // ERROR : Must never occur
            errx(1,
                 "lexer: get_next_token: impossible error while getting the "
                 "op_type");
        struct token *op_token = token_new_op(op_type);
        return op_token;
    }
    else
    {
        // The pretoken is a PRETOKEN_WORD

        size_t current_line_index = lexer->line_index;
        lexer->line_index += 1;

        // 'if' -> RW_IF; 'word' -> RW_UNKNOWN
        enum rw_type rw_type = match_rw_type(new_pretoken);

        if (rw_type
            != RW_UNKNOWN) // if the syntax matches the one of a reserve word
        {
            if (current_line_index == 0) // first token of the command
            {
                struct token *rw_token = token_new_rw(rw_type);
                return rw_token;
            }
            // TODO other tests in future implementation (for, case)
        }
        // The token is a simple word
        size_t word_size = strlen(new_pretoken->str);
        struct token *word_token = token_new_word(new_pretoken->str, word_size);
        return word_token;
    }
}

struct lexer *lexer_new(struct pretoken_vector *pretokens)
{
    if (!pretokens)
        return NULL;
    struct lexer *new = xmalloc(sizeof(struct lexer));
    new->pretokens = pretokens;
    new->pretoken_index = 0;
    new->line_index = 0;
    new->current_tok = NULL;
    struct token *token = get_next_token(new);
    new->current_tok = token;
    return new;
}

struct token *lexer_peek(struct lexer *lexer)
{
    return lexer->current_tok;
}

void lexer_free(struct lexer *lexer)
{
    free_pretoken_list(lexer->pretokens);
    free(lexer);
}

struct token *lexer_pop(struct lexer *lexer)
{
    enum token_type type = lexer->current_tok->type;
    if (type == TOKEN_EOF)
    {
        return lexer->current_tok;
    }

    struct token *token = get_next_token(lexer);
    struct token *prev = lexer->current_tok;
    lexer->current_tok = token;
    return prev;
}
