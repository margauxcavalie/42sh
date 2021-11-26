#pragma once

#include "pretoken.h"

/**
 * @brief Structure that associates a string with a pretoken type (lookup table)
 * (Simple aggregation)
 */
struct matching_pretoken
{
    const char *str;
    size_t len;
    enum pretoken_type type;
};

/*
 *
 *
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
void append_pretoken_list(struct pretoken_vector *vec, struct pretoken *pretoken);

/**
 * @brief
 * @param input Raw string from the user
 */
struct pretoken_vector *prelexify(char *input);