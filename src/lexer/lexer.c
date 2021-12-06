#include "lexer.h"

#include <err.h>
#include <prelexer/prelexer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>

static struct op_data match_op_type(struct pretoken *new_pretoken)
{
    // initalizes the lookup table
    struct matching_op lookup_table[] = {
        { ";", 1, .data = { OP_SEMICOLON, .data.null = NULL } },
        { "\n", 1, .data = { OP_LINEFEED, .data.null = NULL } },
        { "<", 1,
          .data = { OP_REDIR,
                    .data.redir_data = { REDIR_LESS, STDIN_FILENO } } },
        { ">", 1,
          .data = { OP_REDIR,
                    .data.redir_data = { REDIR_GREAT, STDOUT_FILENO } } },
        { ">>", 2,
          .data = { OP_REDIR,
                    .data.redir_data = { REDIR_DGREAT, STDOUT_FILENO } } },
        { ">|", 2,
          .data = { OP_REDIR,
                    .data.redir_data = { REDIR_CLOBBER, STDOUT_FILENO } } },
        { "<>", 2,
          .data = { OP_REDIR,
                    .data.redir_data = { REDIR_LESSGREAT, STDIN_FILENO } } },
        { ">&", 2,
          .data = { OP_REDIR,
                    .data.redir_data = { REDIR_GREATAND, STDOUT_FILENO } } },
        { "<&", 2,
          .data = { OP_REDIR,
                    .data.redir_data = { REDIR_LESSAND, STDIN_FILENO } } },
        { "!", 1, .data = { OP_NEG, .data.null = NULL } },
        { "||", 2, .data = { OP_OR, .data.null = NULL } },
        { "&&", 2, .data = { OP_AND, .data.null = NULL } },
        { "|", 1, .data = { OP_PIPE, .data.null = NULL } }
    };
    size_t lt_size = sizeof(lookup_table) / sizeof(struct matching_op);

    size_t count = 0;
    while (count < lt_size)
    {
        // if we match the content of the lookup table
        if (strcmp(lookup_table[count].str, new_pretoken->str) == 0)
            return lookup_table[count].data;
        count += 1;
    }
    // if we do not match any of the possible operator
    struct op_data unknown_op = { OP_UNKNOWN, .data.null = NULL };
    return unknown_op;
}

static enum rw_type match_rw_type(struct pretoken *new_pretoken)
{
    // initalizes the lookup table
    struct matching_rw lookup_table[] = {
        { "if", 2, RW_IF },       { "then", 4, RW_THEN },
        { "elif", 4, RW_ELIF },   { "else", 4, RW_ELSE },
        { "fi", 2, RW_FI },       { "while", 5, RW_WHILE },
        { "do", 2, RW_DO },       { "done", 4, RW_DONE },
        { "until", 5, RW_UNTIL }, { "for", 3, RW_FOR },
        { "in", 2, RW_IN }
    };
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
        struct op_data op_data = match_op_type(new_pretoken);
        if (op_data.type == OP_UNKNOWN) // ERROR : Must never occur
            errx(1,
                 "lexer: get_next_token: impossible error while getting the "
                 "op_type");
        struct token *op_token = token_new_op(op_data);
        return op_token;
    }
    else if (new_pretoken->type == PRETOKEN_IONUMBER)
    {
        // The IONUMBER respect this syntax : [0-9]
        int value = new_pretoken->str[0] - '0';
        struct token *op_token = token_new_ionumber(value);
        return op_token;
    }
    else
    {
        // The pretoken is a PRETOKEN_WORD

        size_t current_line_index = lexer->line_index;
        lexer->line_index += 1;

        struct token *previus_token = lexer->current_tok;

        enum rw_type rw_type = RW_UNKNOWN;
        // 'if' -> RW_IF; 'word' -> RW_UNKNOWN
        if (new_pretoken->is_quoted == 0)
            rw_type = match_rw_type(new_pretoken);

        // if the syntax matches the one of a reserved word
        if (rw_type != RW_UNKNOWN)
        {
            bool is_rw = false;
            if (current_line_index == 0) // first token of the command
            {
                is_rw = true;
                lexer->last_rw = rw_type;
            }
            /* TODO && prevtok different than case, for, in*/
            else if ((previus_token->type == TOKEN_RW)
                     && (lexer->last_rw != RW_FOR) && (lexer->last_rw != RW_IN))
            {
                is_rw = true;
                lexer->last_rw = rw_type;
                lexer->line_index = 1;
            }
            // last RW was a FOR and we're at the third position
            else if ((lexer->last_rw == RW_FOR) && (current_line_index == 2))
            {
                is_rw = true;
                lexer->last_rw = rw_type;
            }
            // TODO other tests in future implementation (for, case)

            if (is_rw == true) // the word is a RW
            {
                struct token *rw_token = token_new_rw(rw_type);
                return rw_token;
            }
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
    struct token *tok = lexer_pop(lexer);
    while (tok->type != TOKEN_EOF)
    {
        token_free(tok);
        tok = lexer_pop(lexer);
    }
    token_free(tok);
    free_pretoken_list(lexer->pretokens);
    free(lexer);
}

void lexer_free_without_pretokens(struct lexer *lexer)
{
    struct token *tok = lexer_pop(lexer);
    while (tok->type != TOKEN_EOF)
    {
        token_free(tok);
        tok = lexer_pop(lexer);
    }
    token_free(tok);
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

/**
 * Create a copy of the lexer
 */
struct lexer *save_lexer(struct lexer *lexer)
{
    struct lexer *new = lexer_new(lexer->pretokens);
    while (new->pretoken_index < lexer->pretoken_index)
    {
        token_free(lexer_pop(new));
    }
    return new;
}

void restore_lexer(struct lexer **lexer, struct lexer *save_lexer)
{
    lexer_free_without_pretokens(*lexer);
    *lexer = save_lexer;
}
