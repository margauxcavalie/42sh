#include "var_expansion.h"

#include <hash_map/hash_map.h>
#include <lexer/token.h>
#include <utils/alloc.h>
#define HASH_MAP_SIZE 16

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
 * @brief
 *
 * @param hash_map insert a new pair value / key in the hash map
 * key pair syntax : var=true
 * key : var
 * value : true
 * => $(var) = true
 * @param key name of the var
 * @param value value inside
 * @param updated true if the key was already assiociated with an old value and
 * was updated
 * @return true
 * @return false
 */
bool var_hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                         bool *updated)
{
    return hash_map_insert(hash_map, key, value, updated);
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
    bool res = true;
    if (count <= 1)
        res = c == '\0' || c == '\t' || c == ' ';
    else
        res = c == '\0' || c == '\t' || c == ' ' || c == '$';
    return res;
}

static size_t varlen(char *var)
{
    size_t res = 0;
    size_t count = 1;
    while (var[count] && !is_separator(var[count], count))
    {
        // printf("%c\n", var[count]);
        if (var[count] == '{')
        {
            count++;
            continue;
        }
        if (var[count] == '}')
        {
            break;
        }
        count++;
        res++;
    }
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
 * @brief build a key from they syntax : $var -> var and ${var} -> var
 *
 * @param var
 * @return char*
 */
char *build_key(char *var, int *error)
{
    if (var[0] && var[1] == '\0') // if single $
    {
        return NULL;
    }
    int type = check_brackets(var);
    if (type == -1) // if { without }
    {
        *error = 1;
        return NULL;
    }
    size_t var_len = varlen(var);
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

/**
 * @brief return the value associated to this var,
 * key pair syntax : var=true,
 * key : var,
 * value : true,
 * => ${var} = true
 * @param hash_map
 * @param var
 * @return char*
 */
char *expand_var(struct hash_map *hash_map, char *var, int *error)
{
    if (var == NULL || hash_map == NULL || var[0] != '$')
        return NULL;
    char *key = build_key(var, error);
    if (key == NULL)
        return NULL;
    char *value = hash_map_get(hash_map, key);
    free(key);
    if (value == NULL)
        return "";
    return value;
}
