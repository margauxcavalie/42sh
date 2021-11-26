#pragma once

#include <unistd.h>

enum pretoken_type
{
    PRETOKEN_OPERATOR,
    PRETOKEN_WORD,
    PRETOKEN_EOF
};

struct pretoken
{
    enum pretoken_type type;
    char *str; // if type == PRETOKEN_EOF then NULL
};

/**
 * \brief Allocate a new token
 */
struct token *pretoken_new(enum token_type type);

/**
 * \brief Frees a token
 */
void pretoken_free(struct token *token);
