#pragma once

#include <unistd.h>

enum pretoken_type
{
    PRETOKEN_OPERATOR,
    PRETOKEN_WORD,
    PRETOKEN_EOF,
    PRETOKEN_ERROR
};

struct pretoken
{
    enum pretoken_type type;
    char *str; // if type == PRETOKEN_EOF then NULL
};

/**
 * \brief Allocate a new token
 */
struct pretoken *pretoken_new(enum pretoken_type type, const char *str,
                              size_t len);

/**
 * \brief Frees a token
 */
void pretoken_free(struct pretoken *pretoken);
