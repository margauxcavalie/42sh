#include <err.h>
#include <prelexer/prelexer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>
#include <utils/vec.h>

// initializes the lookup table (by decending order (size))
const struct pretoken_operator ops[] = { { ">&", 2 }, { "<&", 2 }, { ";;", 2 },
                                         { ">>", 2 }, { "&&", 2 }, { "||", 2 },
                                         { "|", 1 },  { ">|", 2 }, { "<>", 2 },
                                         { "\n", 1 }, { ";", 1 },  { ">", 1 },
                                         { "<", 1 },  { "{", 1 },  { "}", 1 },
                                         { "(", 1 },  { ")", 1 },  { "#", 1 } };
#define nb_ops 18

static bool is_operator(const char *str)
{
    size_t i = 0;
    while (i < nb_ops)
    {
        if (!strncmp(ops[i].str, str, ops[i].len))
        {
            return true;
        }
        i++;
    }
    return false;
}

size_t get_index_of_op(const char *str)
{
    size_t i = 0;
    while (i < nb_ops)
    {
        if (!strncmp(ops[i].str, str, ops[i].len))
        {
            return i;
        }
        i++;
    }
    return 0;
}

static bool is_separator(char c)
{
    bool res = c == '\0' || c == '\t' || c == ' ';
    return res;
}

static bool is_ionumber(const char *str)
{
    if (str[0] == '\0' || str[1] == '\0')
        return false;
    if (str[0] >= '0' && str[0] <= '9') // is a digit ?
    {
        if (str[1] == '<' || str[1] == '>')
            return true;
    }
    return false;
}

struct pretoken_vector *init_pretoken_list()
{
    struct pretoken_vector *new = xmalloc(sizeof(struct pretoken_vector));
    new->size = 0;
    new->capacity = 8;
    new->data = xmalloc(sizeof(struct pretoken *) * new->capacity);
    return new;
}

void free_pretoken_list(struct pretoken_vector *vec)
{
    for (size_t i = 0; i < vec->size; i++)
    {
        pretoken_free(vec->data[i]);
    }
    free(vec->data);
    free(vec);
}

void append_pretoken_list(struct pretoken_vector *vec,
                          struct pretoken *pretoken)
{
    if (vec->size + 1 > vec->capacity)
    {
        vec->capacity = (vec->size + 1) * 2;
        vec->data =
            xrealloc(vec->data, sizeof(struct pretoken *) * vec->capacity);
    }
    vec->data[vec->size] = pretoken;
    vec->size += 1;
}

/**
 * @brief skip until a line finished
 * return token EOF or NEWLINE
 */
static struct pretoken *skip_comment(const char *str, size_t *size)
{
    while (str[*size] != '\0' && str[*size] != '\n')
    {
        *size += 1;
    }
    if (str[*size] == '\n')
    {
        size_t op_index = get_index_of_op("\n");
        return pretoken_new(PRETOKEN_OPERATOR, ops[op_index].str,
                            ops[op_index].len);
    }
    else
    {
        return pretoken_new(PRETOKEN_EOF, NULL, 0);
    }
}

/**
 * @brief skip until a quote is found return an error if not
 *
 */
static bool skip_quotes(const char *str, int *counter, char c, int *not_ended)
{
    // printf("dep : %c\n", str[*counter]);
    *counter += 1;
    while (str[*counter] != '\0' && str[*counter] != c)
    {
        // printf("char : %c\n", str[*counter]);
        if (c == '"' && str[*counter] == '\\')
            *counter += 1;
        if (str[*counter] == '\0')
        {
            *not_ended = 1;
            return false;
        }
        *counter += 1;
    }
    if (str[*counter] == '\0')
    {
        *not_ended = 1;
        return false;
    }
    //*counter += 1;
    // printf("end : %c\n", str[*counter]);
    return true;
}

/**
 *  @brief returns the first word found in str
 */
static char *get_word(const char *str, size_t *size, int *not_ended)
{
    int counter = 0;
    // init the vec
    struct vec *curr_token = xmalloc(sizeof(struct vec));
    vec_init(curr_token);

    while (!(is_separator(str[counter])) && !is_operator(str + counter))
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
            if (skip_quotes(str, &counter, c, not_ended) == false)
            {
                // add the quoted content to the curr token
                for (int i = tmp; i < counter; i++)
                    vec_push(curr_token, str[i]);
            }
            else
            {
                // add the quoted content to the curr token
                for (int i = tmp; i <= counter; i++)
                    vec_push(curr_token, str[i]);
                counter += 1;
            }
        }
        else // add the char to the token and proceed to the next one
        {
            if (str[counter] == '\\') // skip if backslash
            {
                vec_push(curr_token, str[counter]);
                counter += 1;
                if (str[counter] == '\0')
                {
                    *not_ended = 1;
                    break;
                }
                vec_push(curr_token, str[counter]);
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
    *size += counter; // update the size
    return word;
}

/**
 *@brief
 *
 * @param str:
 * @param size:
 */
struct pretoken *get_next_pretoken(const char *str, size_t *size,
                                   int *not_ended)
{
    // skip all separators
    while (is_separator(str[0]) && str[0] != '\0')
    {
        *size += 1;
        str += 1;
    }
    size_t i = 0;
    while (i < nb_ops)
    {
        struct pretoken_operator pretok_op = ops[i];
        // check if '!' is followed by a space
        /*if ((!strcmp(pretok_op.str, "!"))
            && (!strncmp(pretok_op.str, str, pretok_op.len)))
        {
            if (!strncmp(str + 1, " ", 1))
            {
                *size += pretok_op.len;
                if (strcmp(pretok_op.str, "#") == 0) // if is a comment
                {
                    return skip_comment(str, size);
                }
                struct pretoken *new = pretoken_new(
                    PRETOKEN_OPERATOR, pretok_op.str, pretok_op.len);
                return new;
            }
            else
            {
                break;
            }
        }*/
        if (!strncmp(pretok_op.str, str, pretok_op.len))
        {
            // if we found an operator
            *size += pretok_op.len;
            if (strcmp(pretok_op.str, "#") == 0) // if is a comment
            {
                return skip_comment(str, size);
            }
            struct pretoken *new =
                pretoken_new(PRETOKEN_OPERATOR, pretok_op.str, pretok_op.len);
            return new;
        }
        i++;
    }
    if (str[0] == '\0')
    {
        struct pretoken *new = pretoken_new(PRETOKEN_EOF, NULL, 0);
        return new;
    }
    if (is_ionumber(str))
    {
        struct pretoken *new = pretoken_new(PRETOKEN_IONUMBER, str, 1);
        *size += 1;
        return new;
    }
    // else it's a word
    struct pretoken_operator *operators_cpy =
        xmalloc(sizeof(struct pretoken_operator) * nb_ops);
    memcpy(operators_cpy, ops, sizeof(struct pretoken_operator) * nb_ops);
    char *word = get_word(str, size, not_ended);
    if (word == NULL)
    {
        struct pretoken *new = pretoken_new(PRETOKEN_ERROR, NULL, 0);
        free(operators_cpy);
        return new;
    }
    size_t word_size = strlen(word);
    struct pretoken *new = pretoken_new(PRETOKEN_WORD, word, word_size);
    free(word);
    free(operators_cpy);
    return new;
}

struct pretoken_vector *prelexify(char const *input, int *not_ended)
{
    struct pretoken_vector *vec = init_pretoken_list();
    size_t size = 0;
    struct pretoken *pretok = get_next_pretoken(input, &size, not_ended);
    while (pretok->type != PRETOKEN_EOF && pretok->type != PRETOKEN_ERROR)
    {
        append_pretoken_list(vec, pretok);
        pretok = get_next_pretoken(input + size, &size, not_ended);
    }
    if (pretok->type == PRETOKEN_ERROR)
    {
        append_pretoken_list(vec, pretok);
        free_pretoken_list(vec);
        return NULL;
    }
    append_pretoken_list(vec, pretok);
    return vec;
}

void print_pretoken_list(struct pretoken_vector *vec)
{
    char *pretoken_type_str[] = { "OP", "WORD", "IO", "EOF" };
    printf("Pretoken list: [");
    for (size_t i = 0; i < vec->size; i++)
    {
        int curr_type = vec->data[i]->type;
        char *curr_str = vec->data[i]->str;
        char *type_str = pretoken_type_str[curr_type];
        if (curr_type == PRETOKEN_EOF)
            printf("%s(NULL)", type_str);
        else
            printf("%s('%s')", type_str, curr_str);
        if (i != vec->size - 1)
            printf(", ");
    }
    printf("]\n");
}
