#include "lexer.h"

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

static bool is_separator(char c)
{
    bool res = c == '\0' || is_space(c) || c == '\n' || c == ';' || c == '\'';
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

/*
 *@brief
 *
 * @param str:
 * @param size:
 */
struct token *get_next_token(const char *str, size_t *size)
{
    // initializes the lookup table
    struct matching_token lookup_table[] = {
        { "if", 2, TOKEN_IF },       { "then", 4, TOKEN_THEN },
        { "elif", 4, TOKEN_ELIF },   { "else", 4, TOKEN_ELSE },
        { "fi", 2, TOKEN_FI },       { ";", 1, TOKEN_SEMICOLON },
        { "\n", 1, TOKEN_LINEFEED }, { "\'", 1, TOKEN_SINGLE_QUOTE },
        { "\0", 0, TOKEN_EOF },
    };

    // gets the number of elements of the lookup table
    size_t nb_tokens = sizeof(lookup_table) / sizeof(struct matching_token);

    // skip all spaces
    while (is_space(str[0]))
    {
        *size += 1;
        str += 1;
    }
    size_t i = 0;
    while (i < nb_tokens)
    {
        struct matching_token mt = lookup_table[i];
        if (are_strings_equal(mt.str, str, mt.len))
        {
            // if we found the token type
            *size += mt.len;
            struct token *new_token = token_new(mt.type);
            return new_token;
        }
        i++;
    }
    // if a word was found
    struct token *new_token = token_new(TOKEN_WORD);
    char *word = get_word(str, size);
    new_token->word = word;
    return new_token;
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

    size_t size = 0;
    struct token *token = get_next_token(lexer->input + lexer->pos, &size);
    struct token *prev = lexer->current_tok;
    lexer->current_tok = token;
    lexer->pos += size;
    return prev;
}
