#include <builtins/builtins.h>
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <utils/vec.h>

static void set_key_value_pair(char *word, char **key, char **value)
{
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    size_t i = 0;
    for (; word[i] && word[i] != '='; i++)
    {
        vec_push(vec, word[i]);
    }
    *key = vec_cstring(vec);
    free(vec);
    vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    i += 1;
    for (; word[i] && word[i]; i++)
    {
        vec_push(vec, word[i]);
    }
    *value = vec_cstring(vec);
    free(vec);
    // printf("%s\n", ast->key);
    // printf("%s\n", ast->value);
}

static bool is_a_spec_char(char c)
{
    bool res = c == '$' || c == '@' || c == '?' || c == '*' || c == '#';
    return res;
}

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

static bool check_valid_name(char *str)
{
    bool res = true;
    if ((is_a_spec_char(str[0]) || isdigit(str[0])))
    {
        return false;
    }
    for (int i = 0; str[i] != '\0'; i++)
    {
        res = res && !is_separator(str[i], i);
    }
    return res;
}

int builtin_export(int argc, char **argv, struct runtime *rt)
{
    char *key = NULL;
    char *value = NULL;
    int status_code = 0;
    for (int i = 1; i < argc; i++)
    {
        if (check_assignement_word(argv[i]))
        {
            set_key_value_pair(argv[i], &key, &value);
            var_hash_map_insert_env(rt->variables, key, value);
        }
        else if (check_valid_name(argv[i]))
        {
            // setup the key
            char *tmp = zalloc(sizeof(char) * (strlen(argv[i]) + 1));
            strcpy(tmp, argv[i]);
            struct var *oldvalue = hash_map_get(rt->variables, argv[i]);
            if (oldvalue != NULL) // if the var already exists
            {
                char *tmp2 =
                    zalloc(sizeof(char) * (strlen(oldvalue->value) + 1));
                strcpy(tmp2, oldvalue->value);
                var_hash_map_insert_env(rt->variables, tmp, tmp2);
            }
            else // if not
            {
                char *tmp2 = zalloc(sizeof(char));
                var_hash_map_insert_env(rt->variables, tmp, tmp2);
            }
        }
        else
        {
            warnx("export: %s: not a valid identifier", argv[i]);
            status_code = 1;
        }
    }
    return status_code;
}
