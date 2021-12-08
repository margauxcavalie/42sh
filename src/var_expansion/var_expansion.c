#include "var_expansion.h"

#include <ctype.h>
#include <hash_map/hash_map.h>
#include <lexer/token.h>
#include <stdlib.h>
#include <utils/alloc.h>
#include <utils/vec.h>
#define HASH_MAP_SIZE 32

/**
 * @brief init the hash_map storing our variables
 *
 * @return struct hash_map*
 */
struct hash_map *var_hash_map_init(void)
{
    struct hash_map *hash_map = hash_map_init(HASH_MAP_SIZE);
    return hash_map;
}

/**
 * @brief insert a new pair value / key in the hash map
 * key pair syntax : var=true
 * key : var
 * value : true
 * => $(var) = true
 * @param hash_map
 * @param key name of the var
 * @param value value inside
 * @return true
 * @return false
 */
bool var_hash_map_insert(struct hash_map *hash_map, char *key, char *value)
{
    return hash_map_insert(hash_map, key, value, free);
}

/**
 * @brief free an hash map full of variables
 *
 * @param hash_map
 */
void var_hash_map_free(struct hash_map *hash_map)
{
    hash_map_free(hash_map, free);
}

static bool is_a_spec_char(char c)
{
    bool res = c == '$' || c == '@' || c == '?' || c == '*' || c == '#';
    return res;
}

/**
 * @brief
 * $ is a separator if not in first place here
 * @param c
 * @return true
 * @return false
 */
static bool is_separator(char c, int count)
{
    // printf("%c\n", c);
    bool res = true;
    if (count <= 1)
    {
        res = !isalnum(c) && !is_a_spec_char(c) && c != '{' && c != '_';
    }
    else
    {
        res = !isalnum(c) && c != '_';
    }
    return res;
}

/**
 * @brief
 *
 */
int check_valid_substitution(char *str, size_t count)
{
    int res = 1;
    if ((is_a_spec_char(str[0]) || isdigit(str[0])) && count > 3)
    {
        return 0;
    }
    for (size_t i = 0; str[i] != '}'; i++)
    {
        res = res && !is_separator(str[i], i);
    }
    return res;
}

/**
 * @brief len of a var behind a $
 *
 * @param var
 * @param counter
 * @return size_t
 */
static size_t varlen(char *var, size_t *counter)
{
    size_t res = 0;
    size_t count = 1;
    if (isdigit(var[count]))
    {
        res++;
        count++;
        *counter += count;
        return res;
    }
    while (var[count] && !is_separator(var[count], count))
    {
        // printf("%c\n", var[count]);
        if (var[count] == '{')
        {
            count++;
            size_t dep = count;
            while (var[count] && var[count] != '}')
            {
                // printf("%c\n", var[count]);
                count++;
                res++;
            }
            if (var[count] == '}')
            {
                if (check_valid_substitution(var + dep, count) == 0)
                {
                    return 0;
                }
                count++;
                *counter += count;
                return res;
            }
            break;
        }
        count++;
        res++;
    }
    *counter += count;
    // printf("%zu\n", res);
    return res;
}

/**
 * @brief return -1 if error, 0 if not bracket, 1 if bracket
 *
 * @param str
 * @return int
 */
static int check_brackets(const char *str)
{
    size_t counter = 1;
    if (str[counter] == '{')
    {
        while (str[counter] && str[counter] != '}')
        {
            // printf("char : %c\n", str[counter]);
            if (str[counter] == '\0')
                return -1;
            counter += 1;
        }
        if (str[counter] == '\0')
            return -1;
        return 1;
    }
    return 0;
}

/**
 * @brief return -1 if bracket error, 0 if not
 *
 * @param str
 * @return int
 */
int check_all_brackets(char *str)
{
    if (str == NULL || str[0] == '\0')
        return 0;
    size_t count = 0;
    while (str[count])
    {
        if (str[count] && str[count + 1] == '\0') // if single $ at the end
        {
            return 0;
        }
        if (str[count] == '\\') // skip if backslash
        {
            if (str[count + 1] != '\0')
            {
                count += 1;
            }
        }
        else if (str[count] == '$' && str[count + 1] != ' ')
        {
            int res = check_brackets(str + count);
            size_t tmp = 0;
            size_t var_len = varlen(str + count, &tmp);
            if (var_len == 0) // case ${} is an error
            {
                return -1;
            }
            if (res == -1)
                return -1;
        }
        count++;
    }
    return 0;
}

/**
 * @brief build a key from they syntax : $var -> var and ${var} -> var
 *
 * @param var
 * @return char*
 */
char *build_key(char *var, int *error, size_t *counter)
{
    if (var[0] && (is_separator(var[1], 0))) // if single $
    {
        *counter += 1;
        return NULL;
    }
    int type = check_brackets(var);
    if (type == -1) // if { without }
    {
        *error = 1;
        return NULL;
    }
    size_t var_len = varlen(var, counter);
    if (type == 0) // if no {}
    {
        char *res = zalloc(sizeof(char) * (var_len + 1));
        strncpy(res, var + 1, var_len);
        return res;
    }
    else // if {}
    {
        if (var_len == 0) // case ${} is an error
        {
            *error = 1;
            return NULL;
        }
        char *res = zalloc(sizeof(char) * (var_len + 1));
        strncpy(res, var + 2, var_len);
        return res;
    }
}
#include <stdio.h>
/**
 * @brief return the value associated to this var,
 * key pair syntax : var=true,
 * key : var,
 * value : true,
 * => ${var} = true
 * @param var
 * @return char*
 */
char *expand_var(struct hash_map *hash_map, char *var, int *error,
                 size_t *counter)
{
    if (var == NULL || var[0] != '$')
    {
        return NULL;
    }
    char *key = build_key(var, error, counter);
    if (key == NULL)
        return NULL;
    char *value = hash_map_get(hash_map, key);
    free(key);
    if (value == NULL)
        return "";
    return value;
}

/**
 * @brief The <backslash> shall retain its special meaning as an escape
 * character if followe by : $   `   "   \   <newline>
 *
 * @param c
 * @return int
 */
static int can_be_escaped(char c)
{
    int res = c == '$' || c == '`' || c == '"' || c == '\\' || c == '\n';
    return res;
}

/**
 * @brief expands all the var within the string and return a new string
 * key pair syntax : var=true,
 * key : var,
 * value : true,
 * => ${var} = true
 * @param var
 * @return char*
 */
char *expand_all_string(struct hash_map *hash_map, char *str, int *error,
                        size_t *counter)
{
    if (str[0] != '\"')
    {
        *error = 1;
        return NULL;
    }
    size_t count = 1; // skip the first \"
    // init the vec
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    while (str[count] != '\0' && str[count] != '\"')
    {
        // skip until we find a $
        while (str[count] != '\0' && str[count] != '\"' && str[count] != '$')
        {
            if (str[count] == '\\') // skip if backslash
            {
                if (str[count + 1] != '\0' && can_be_escaped(str[count + 1]))
                {
                    count += 1;
                }
            }
            // printf("%c\n", str[counter]);
            vec_push(vec, str[count]);
            count += 1;
        }
        if (str[count] == '\0' || str[count] == '\"')
        {
            break;
        }
        // else it's a word
        char *word = expand_var(hash_map, str + count, error, &count);
        /*printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
        if (word != NULL)
            printf("%s\n", word);
        printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
        printf("error = %d\n", *error);*/
        if (word == NULL)
        {
            if (*error)
            {
                // printf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeerror\n");
                vec_destroy(vec);
                free(vec);
                return NULL;
            }
            /*printf("$ will be added\n");
            printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");*/
            vec_push(vec, '$');
            /*printf("$ added\n");
            printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");*/
            continue;
        }
        // printf("word != NULL\n");
        size_t word_size = strlen(word);
        for (size_t i = 0; i < word_size; i++)
        {
            vec_push(vec, word[i]);
        }
        // printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
        // printf("%s\n", vec_cstring(vec));
        // printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
    }
    if (str[count] == '\0')
    {
        *error = 1;
        vec_destroy(vec);
        free(vec);
        return NULL;
    }
    count++;
    char *res = vec_cstring(vec); // get the string
    *counter += count;
    /*printf("ccccccccccccccccccccccccccccccccccccc\n");
    printf("%s\n", res);
    printf("ccccccccccccccccccccccccccccccccccccc\n");*/
    free(vec); // free the vec preserving the str
    return res;
}
