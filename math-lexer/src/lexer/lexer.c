#include "lexer.h"

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "alloc.h"

static ssize_t get_token_number(const char *str, size_t *size)
{
    char buffer[32] = { 0 };
    int index_buf = 0;
    int counter = 0;
    while (counter < 10 && str[*size] != '\0')
    {
        if (str[*size] < '0' || str[*size] > '9')
        {
            break;
        }
        else
        {
            buffer[index_buf] = str[*size];
            index_buf++;
            *size += 1;
        }
    }
    if (counter == 10)
        return -1;
    return atoi(buffer);
}

static struct token *get_next_token(const char *str, size_t *size)
{
    while (true)
    {
        switch (str[*size])
        {
        case ' ':
            *size += 1;
            break;
        case '+':
            *size += 1;
            return token_new(TOKEN_PLUS);
        case '-':
            *size += 1;
            return token_new(TOKEN_MINUS);
        case '*':
            *size += 1;
            return token_new(TOKEN_MUL);
        case '/':
            *size += 1;
            return token_new(TOKEN_DIV);
        case '(':
            *size += 1;
            return token_new(TOKEN_LEFT_PAR);
        case ')':
            *size += 1;
            return token_new(TOKEN_RIGHT_PAR);
        case '\0':
            *size += 1;
            return token_new(TOKEN_EOF);
        default: {
            char c = str[*size];
            if (c < '0' || c > '9')
            {
                *size += 1;
                return token_new(TOKEN_ERROR);
            }
            else
            {
                size_t size_number = 0;
                ssize_t value = get_token_number(str + *size, &size_number);
                *size += size_number;
                if (value == -1)
                {
                    return token_new(TOKEN_ERROR);
                }
                struct token *token = token_new(TOKEN_NUMBER);
                token->value = value;
                return token;
            }
        }
        }
    }
}

struct lexer *lexer_new(const char *input)
{
    if (!input)
        return NULL;
    struct lexer *new = xmalloc(sizeof(struct lexer));
    new->input = input;
    new->pos = 0;
    new->current_tok = NULL;
    struct token *token = get_next_token(input, &new->pos);
    new->current_tok = token;
    return new;
}

struct token *lexer_peek(struct lexer *lexer)
{
    if (lexer->current_tok->type == TOKEN_ERROR)
        fprintf(
            stderr,
            "lexer: token is not valid cheh nik zebi, jsuis zaef, nik 3lik\n");
    return lexer->current_tok;
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

struct token *lexer_pop(struct lexer *lexer)
{
    enum token_type type = lexer->current_tok->type;
    if (type == TOKEN_EOF)
    {
        return lexer->current_tok;
    }
    if (type == TOKEN_ERROR)
        fprintf(
            stderr,
            "lexer: token is not valid cheh nik zebi, jsuis zaef, nik 3lik\n");
    size_t size = 0;
    struct token *token = get_next_token(lexer->input + lexer->pos, &size);
    struct token *prev = lexer->current_tok;
    lexer->current_tok = token;
    lexer->pos += size;
    return prev;
}
