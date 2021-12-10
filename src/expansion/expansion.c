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
static bool skip_quotes(const char *str, size_t *counter, char c)
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
static char *get_word(char *str, size_t *size, struct runtime *rt)
{
    size_t counter = 0;
    // init the vec
    struct vec *curr_token = xmalloc(sizeof(struct vec));
    vec_init(curr_token);

    while (str[counter] != '\0')
    {
        // quotes
        if (str[counter] == '\'')
        {
            size_t tmp = counter;
            if (skip_quotes(str, &counter, '\'') == false)
            {
                // ERROR: quote is not paired
                vec_destroy(curr_token);
                free(curr_token);
                return NULL;
            }
            else
            {
                // add the quoted content to the curr token
                for (size_t i = tmp + 1; i < counter; i++)
                    vec_push(curr_token, str[i]);
            }
            counter += 1;
            // printf("counter : %zu", counter);
        }
        else if (str[counter] == '\"')
        {
            // mark the dbl quotes
            int error = 0;
            char *res = expand_all_string(rt->variables, str + counter, &error,
                                          &counter);
            counter -= 1;
            if (res == NULL)
            {
                // ERROR: quote is not paired or var error
                vec_destroy(curr_token);
                free(curr_token);
                return NULL;
            }
            else
            {
                // add the quoted content to the curr token
                for (size_t i = 0; res[i]; i++)
                    vec_push(curr_token, res[i]);
            }
            free(res);
            counter += 1;
            // printf("counter : %zu", counter);
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

struct vector *expands_word(char *word, struct runtime *rt)
{
    size_t size = 0;
    char *str = get_word(word, &size, rt);
    if (!str)
        return NULL;
    // init the vec
    struct vector *vec = vector_init(1);
    vector_append(vec, str);

    return vec;
}
