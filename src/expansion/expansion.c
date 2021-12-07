#include "expansion.h"

#include <lexer/token.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>
#include <utils/vec.h>
#include <vector/vector.h>

/**
 * @brief skip until a quote is found return an error if not
 *
 */
static bool skip_quotes(const char *str, int *counter, char c)
{
    // printf("dep : %c\n", str[*counter]);
    *counter += 1;
    while (str[*counter] && str[*counter] != c)
    {
        // printf("char : %c\n", str[*counter]);
        if (str[*counter] == '\0')
            return false;
        *counter += 1;
    }
    if (str[*counter] == '\0')
        return false;
    // printf("end : %c\n", str[*counter]);
    return true;
}

/**
 *  @brief returns the first word found in str
 */
static char *get_word(const char *str, size_t *size)
{
    int counter = 0;
    // init the vec
    struct vec *curr_token = xmalloc(sizeof(struct vec));
    vec_init(curr_token);

    while (str[counter] != '\0')
    {
        char c = 0;
        // quotes
        if (str[counter] == '\'')
        {
            // mark the quotes
            c = '\'';
        }
        if (str[counter] == '\"')
        {
            // mark the dbl quotes
            c = '\"';
        }
        if (c != 0)
        {
            int tmp = counter;
            if (skip_quotes(str, &counter, c) == false)
            {
                // ERROR: quote is not paired
                vec_destroy(curr_token);
                free(curr_token);
                return NULL;
            }
            else
            {
                // add the quoted content to the curr token
                for (int i = tmp + 1; i < counter; i++)
                    vec_push(curr_token, str[i]);
            }
            counter += 1;
        }
        else // add the char to the token and proceed to the next one
        {
            if (str[counter] == '\\') // skip if backslash
            {
                if (str[counter + 1] != '\0' && str[counter + 1] != EOF)
                {
                    counter += 1;
                    vec_push(curr_token, str[counter]);
                }
            }
            else
            {
                vec_push(curr_token, str[counter]);
            }
            counter += 1;
        }
    }

    char *word = vec_cstring(curr_token); // get the string
    free(curr_token); // free the vec preserving the str
    *size += counter;
    return word;
}

struct vector *expands_word(const char *word,
                            __attribute__((unused)) struct runtime *rt)
{
    size_t size = 0;
    char *str = get_word(word, &size);
    if (!str)
        return NULL;
    // init the vec
    struct vector *vec = vector_init(1);
    vector_append(vec, str);

    return vec;
}
