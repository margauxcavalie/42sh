#pragma once

#include <unistd.h>

enum token_type
{
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_SEMICOLON,
    TOKEN_LINEFEED,
    TOKEN_SINGLE_QUOTE,
    TOKEN_WORD,
    TOKEN_EOF
};

struct token
{
    enum token_type type;
    char *word; // NULL if type != TOKEN_WORD
};

/**
 * \brief Allocate a new token
 */
struct token *token_new(enum token_type type);

/**
 * \brief Frees a token
 */
void token_free(struct token *token);
