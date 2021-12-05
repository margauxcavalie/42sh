#include "var_expansion.h"

#include <hash_map/hash_map.h>
#include <lexer/token.h>
#include <utils/alloc.h>
#include <utils/vec.h>
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
 * @brief insert a new pair value / key in the hash map
 * key pair syntax : var=true
 * key : var
 * value : true
 * => $(var) = true
 * @param hash_map
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

static size_t varlen(char *var, size_t *counter)
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
            count++;
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
    if (var[0] && (var[1] == '\0' || var[1] == ' ')) // if single $
    {
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
char *expand_var(struct hash_map *hash_map, char *var, int *error,
                 size_t *counter)
{
    if (var == NULL || hash_map == NULL || var[0] != '$')
        return NULL;
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
 * @param hash_map
 * @param var
 * @return char*
 */
char *expand_all_string(struct hash_map *hash_map, char *str, int *error)
{
    size_t counter = 0;
    // init the vec
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    while (str[counter] != '\0')
    {
        // skip until we find a $
        while (str[counter] != '\0' && str[counter] != '$')
        {
            if (str[counter] == '\\') // skip if backslash
            {
                if (str[counter + 1] != '\0'
                    && can_be_escaped(str[counter + 1]))
                {
                    counter += 1;
                }
            }
            //printf("%c\n", str[counter]);
            vec_push(vec, str[counter]);
            counter += 1;
        }
        if (str[counter] == '\0')
        {
            break;
        }
        // else it's a word
        char *word = expand_var(hash_map, str + counter, error, &counter);
        /*printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
        if (word != NULL)
            printf("%s\n", word);
        printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
        printf("error = %d\n", *error);*/
        if (word == NULL)
        {
            if (*error)
            {
                //printf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeerror\n");
                vec_destroy(vec);
                free(vec);
                return NULL;
            }
            /*printf("$ will be added\n");
            printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");*/
            vec_push(vec, '$');
            counter++;
            /*printf("$ added\n");
            printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");*/
            continue;
        }
        //printf("word != NULL\n");
        size_t word_size = strlen(word);
        for (size_t i = 0; i < word_size; i++)
        {
            vec_push(vec, word[i]);
        }
        // printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
        // printf("%s\n", vec_cstring(vec));
        // printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
    }
    char *res = vec_cstring(vec); // get the string
    /*printf("ccccccccccccccccccccccccccccccccccccc\n");
    printf("%s\n", res);
    printf("ccccccccccccccccccccccccccccccccccccc\n");*/
    free(vec); // free the vec preserving the str
    return res;
}
