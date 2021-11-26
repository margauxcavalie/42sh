#include "prelexer/prelexer.h"

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>


/**
 *  @brief return true if character is a space
 */
static bool is_space(char c)
{
    bool res = c == ' ' || c == '\t' || c == '\r';
    return res;
}

/**
 *  @brief return true if the given string matches with the pattern
 */
static bool are_strings_equal(const char *pattern, const char *str, size_t len)
{
    bool res = false;
    if (*pattern == '\0' && *str == '\0')
        return true;
    if (strncmp(str, pattern, len) == 0)
    {
        char c = str[len];
        if (is_separator(c))
            res = true;
    }
    return res;
}

struct pretoken_vector *init_pretoken_list()
{
    struct pretoken_vector *new = xmalloc(sizeof(struct pretoken_vector));
    new->size = 0;
    new->capacity = 8;
    new->data = xmalloc(sizeof(struct pretoken *) * capacity);
    return new;
}

void free_pretoken_list(struct pretoken_vector *vec)
{
    free(vec->data);
    free(vec);
}

void append_pretoken_list(struct pretoken_vector *vec, struct pretoken *pretoken)
{
    if (vec->size + 1 > vec->capacity)
    {
        vec->capacity = (vec->size + 1) * 2;
        vec->data = xrealloc(vec->data, vec->capacity);
    }
    vec->data[vec->size] = pretoken;
    vec->size += 1;
}

static struct pretoken *checkoperator(char *input, size_t *size)
{
    char *separators[] = { "\n", "\t", " ", ";" };
    char *operators[] = { "\n", ";" };
    size_t nb_separator = sizeof(separators) / sizeof(char);
    size_t nb_operators = sizeof(operators) / sizeof(char);
    for (size_t i = 0; i < nb_separator; i++)
    {
        if (separators[i][0] == *(input + size))
        {
            struct pretoken *new_pretok = pretoken_new(PRETOKEN_OPERATOR);
            new_pretok->str = xmalloc(sizeof(char));
            new_pretok->str[0] = separators[i][0];
            *size += 1;
            return new_pretok;
        }
    }
    return NULL;
}

static bool is_separator(char c)
{
    bool res = c == '\0' || c == '\n' || c == ';' || c == '\t';
    return res;
}

/*
 *  @brief returns the first word found in str
 */
static char *get_word(const char *str, size_t *size)
{
    int counter = 0;
    while (!(is_separator(str[counter])))
    {
        counter += 1;
    }

    char *word = zalloc((counter + 1) * sizeof(char));
    strncpy(word, str, counter);
    *size += counter;
    return word;
}

struct pretoken_vector *prelexify(char *input)
{
    struct pretoken_vector *vec = init_pretoken_list();
    size_t size = 0;
    while (*(input + size) != '\0')
    {
        struct pretoken *pre_tok = checkoperator(input, &size);
        //if it's an operator then append it into the pretoken_vector
        if (pre_tok != NULL)
        {
            append_pretoken_list(vec, pre_tok);
        }
        else //it's a word
        {
            char *word = get_word(input + size, &size);
            struct pretoken *word_pretok = pretoken_new(PRETOKEN_WORD);
            word_pretok->str = word;
            append_pretoken_list(vec, word_pretok);
        }
    }
    //end of input so append pretoken EOF
    struct pretoken *eof_pretok = pretoken_new(PRETOKEN_EOF);
    eof_pretok->str = xmalloc(sizeof(char));
    eof_pretok->str[0] = '\0';
    append_pretoken_list(vec, eof_pretok);
    return vec;
}