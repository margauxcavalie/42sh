#include "var_expansion.h"

#include <ctype.h>
#include <hash_map/hash_map.h>
#include <lexer/token.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utils/alloc.h>
#include <utils/vec.h>

#define HASH_MAP_SIZE 32

/**
 * @brief function passed to the generci hash table
 *
 * @param var
 */
static void free_var(void *var)
{
    struct var *vartmp = var;
    free(vartmp->value);
    free(vartmp);
}

/**
 * @brief to init a var
 *
 * @param value
 * @return struct var*
 */
static struct var *init_var(char *value, int isenv)
{
    struct var *new_var = xmalloc(sizeof(struct var));
    new_var->value = value;
    new_var->isenv = isenv;
    return new_var;
}

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
    int isenv = 0;
    // update env if necessary
    struct var *oldvar = hash_map_get(hash_map, key);
    if (oldvar != NULL)
    {
        if (oldvar->isenv)
        {
            isenv = 1;
            setenv(key, value, 1);
        }
    }
    struct var *var_value = init_var(value, isenv);
    return hash_map_insert(hash_map, key, var_value, free_var);
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
bool var_hash_map_insert_env(struct hash_map *hash_map, char *key, char *value)
{
    int isenv = 1;
    struct var *var_value = init_var(value, isenv);
    setenv(key, value, 1);
    return hash_map_insert(hash_map, key, var_value, free_var);
}

/**
 * @brief remove the value associated to the key
 *
 * @param hash_map
 * @param key
 * @return true
 * @return false
 */
bool var_hash_map_remove(struct hash_map *hash_map, char *key)
{
    unsetenv(key);
    return hash_map_remove(hash_map, key, free_var);
}

/**
 * @brief free an hash map full of variables
 *
 * @param hash_map
 */
void var_hash_map_free(struct hash_map *hash_map)
{
    hash_map_free(hash_map, free_var);
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
 * @brief check if str subsitued is a valid substitution
 *
 */
static int check_valid_substitution(char *str, size_t count)
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

/**
 * @brief return a special str if the var is special
 *
 * @param key
 * @return char*
 */
char *handle_special_var(struct hash_map *hash_map, char *key)
{
    if (key)
    {
        if (strcmp(key, "$") == 0)
        {
            int pid = getpid();
            char *res = zalloc(sizeof(char) * 10);
            sprintf(res, "%d", pid);
            char *false_key = zalloc(sizeof(char) * 2);
            strcpy(false_key, "$");
            var_hash_map_insert(hash_map, false_key, res);
            return res;
        }
        if (strcmp(key, "RANDOM") == 0)
        {
            srand(time(NULL));
            int random = rand() % 32767;
            char *res = zalloc(sizeof(char) * 10);
            sprintf(res, "%d", random);
            char *false_key = zalloc(sizeof(char) * 7);
            strcpy(false_key, "RANDOM");
            var_hash_map_insert(hash_map, false_key, res);
            return res;
        }
    }
    return NULL;
}

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
    {
        return NULL;
    }
    char *value = handle_special_var(hash_map, key);
    if (value != NULL)
    {
        free(key);
        return value;
    }
    value = NULL;
    struct var *var_value = hash_map_get(hash_map, key);
    if (var_value != NULL)
        value = var_value->value;
    if (value == NULL)
    {
        value = getenv(key);
        if (value == NULL)
        {
            value = "";
        }
    }
    free(key);
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
    size_t count = 1; //  skip the first \"
    // init the vec
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    vec_push(vec, str[0]); // new push the first \"
    while (str[count] != '\0' && str[count] != '\"')
    {
        // skip until we find a $
        while (str[count] != '\0' && str[count] != '\"' && str[count] != '$')
        {
            if (str[count] == '\\') // skip if backslash
            {
                if (str[count + 1] != '\0' && can_be_escaped(str[count + 1]))
                {
                    vec_push(vec, str[count]); // new
                    count += 1;
                }
            }
            vec_push(vec, str[count]);
            count += 1;
        }
        if (str[count] == '\0' || str[count] == '\"')
        {
            break;
        }
        // else it's a word
        char *word = expand_var(hash_map, str + count, error, &count);
        if (word == NULL)
        {
            if (*error)
            {
                vec_destroy(vec);
                free(vec);
                return NULL;
            }
            vec_push(vec, '$');
            continue;
        }
        // printf("word != NULL\n");
        size_t word_size = strlen(word);
        for (size_t i = 0; i < word_size; i++)
        {
            vec_push(vec, word[i]);
        }
    }
    if (str[count] == '\0')
    {
        *error = 1;
        vec_destroy(vec);
        free(vec);
        return NULL;
    }
    vec_push(vec, str[count]); // new push the last \"
    count++;
    char *res = vec_cstring(vec); // get the string
    *counter += count;
    free(vec); // free the vec preserving the str
    return res;
}

/**
 * @brief
 * @param var
 * @return char*
 */
char *expand_all_quotes_string(char *str, int *error, size_t *counter)
{
    if (str[0] != '\"')
    {
        *error = 1;
        return NULL;
    }
    size_t count = 1; //  skip the first \"
    // init the vec
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    while (str[count] != '\0' && str[count] != '\"')
    {
        // skip until we find a $
        while (str[count] != '\0' && str[count] != '\"')
        {
            if (str[count] == '\\') // skip if backslash
            {
                if (str[count + 1] != '\0' && can_be_escaped(str[count + 1]))
                {
                    count += 1;
                }
            }
            vec_push(vec, str[count]);
            count += 1;
        }
        if (str[count] == '\0' || str[count] == '\"')
        {
            break;
        }
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
    free(vec); // free the vec preserving the str
    return res;
}

/**
 * @brief check if the str matches the syntax of a name
 *
 * @param str
 * @return bool
 */
static bool check_valid_name(char *str)
{
    bool res = true;
    if ((is_a_spec_char(str[0]) || isdigit(str[0])))
    {
        return false;
    }
    for (size_t i = 0; str[i] != '='; i++)
    {
        res = res && !is_separator(str[i], i);
    }
    return res;
}

/**
 * @brief check if the syntax is one of an assignement word
 *
 * @param word
 * @return true
 * @return false
 */
bool check_assignement_word(char *word)
{
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    size_t i = 0;
    for (; word[i] && word[i] != '='; i++)
    {
        vec_push(vec, word[i]);
    }
    // check if the syntax matches var=
    if (i > 0 && word[i] == '=' && check_valid_name(word))
    {
        vec_destroy(vec);
        free(vec);
        return true;
    }
    vec_destroy(vec);
    free(vec);
    return false;
}
