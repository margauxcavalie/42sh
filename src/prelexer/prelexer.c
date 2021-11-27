#include <err.h>
#include <prelexer/prelexer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>

static bool is_operator(const char *str, struct pretoken_operator *ops,
                        size_t nb_ops)
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

static bool is_separator(char c)
{
    bool res = c == '\0' || c == '\t' || c == ' ';
    return res;
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

/*
 *  @brief returns the first word found in str
 */
static char *get_word(const char *str, size_t *size,
                      struct pretoken_operator *ops, size_t nb_ops)
{
    int counter = 0;
    while (!(is_separator(str[counter]))
           && !is_operator(str + counter, ops, nb_ops))
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
struct pretoken *get_next_pretoken(const char *str, size_t *size)
{
    // initializes the lookup table
    struct pretoken_operator operators[] = { { "\n", 1 }, { ";", 1 } };
    // gets the number of elements of the lookup table
    size_t nb_operators = sizeof(operators) / sizeof(struct pretoken_operator);

    // skip all separators
    while (is_separator(str[0]) && str[0] != '\0')
    {
        *size += 1;
        str += 1;
    }
    size_t i = 0;
    while (i < nb_operators)
    {
        struct pretoken_operator pretok_op = operators[i];
        if (!strncmp(pretok_op.str, str, pretok_op.len))
        {
            // if we found an operator
            *size += pretok_op.len;
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
    // else it's a word
    struct pretoken_operator *operators_cpy =
        xmalloc(sizeof(struct pretoken_operator) * nb_operators);
    memcpy(operators_cpy, operators,
           sizeof(struct pretoken_operator) * nb_operators);
    char *word = get_word(str, size, operators_cpy, nb_operators);
    size_t word_size = strlen(word);
    struct pretoken *new = pretoken_new(PRETOKEN_WORD, word, word_size);
    free(word);
    free(operators_cpy);
    return new;
}

struct pretoken_vector *prelexify(char *input)
{
    struct pretoken_vector *vec = init_pretoken_list();
    size_t size = 0;
    struct pretoken *pretok = get_next_pretoken(input, &size);
    while (pretok->type != PRETOKEN_EOF)
    {
        append_pretoken_list(vec, pretok);
        pretok = get_next_pretoken(input + size, &size);
    }
    append_pretoken_list(vec, pretok);
    return vec;
}

void print_pretoken_list(struct pretoken_vector *vec)
{
    char *pretoken_type_str[] = { "OP", "WORD", "EOF" };
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
