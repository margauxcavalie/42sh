#pragma once

#include "pretoken.h"

struct pretoken_operator
{
    const char *str;
    size_t len;
};

/*
 * (complete agregation)
 */
struct pretoken_vector
{
    struct pretoken **data;
    size_t size;
    size_t capacity;
};

/**
 * @brief
 *
 * @return struct pretoken_list *
 */
struct pretoken_vector *init_pretoken_list();

/**
 *
 *
 */
void free_pretoken_list(struct pretoken_vector *vec);

/**
 *
 *
 */
void append_pretoken_list(struct pretoken_vector *vec,
                          struct pretoken *pretoken);

/**
 * @brief
 * @param input Raw string from the user
 */
struct pretoken_vector *prelexify(char const *input, int *not_ended);

/**
 * @brief pretty print of the pretoken list
 */
void print_pretoken_list(struct pretoken_vector *vec);
