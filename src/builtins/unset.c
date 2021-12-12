#include <builtins/builtins.h>
#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>

static bool is_allowed_name(char *str)
{
    if (!strncmp("-", str, 1))
        return false;
    return true;
}

static bool is_allowed_option(char *str)
{
    if (!strcmp("-v", str) || !strcmp("-vf", str) || !strcmp("-fv", str)
        || !strcmp("-f", str))
        return true;
    return false;
}

int builtin_unset(int argc, char **argv, struct runtime *rt)
{
    // just 'unset'
    if (argc == 1)
        return 0;

    // 'unset -vf' || 'unset name'
    else if (argc == 2)
    {
        if (!is_allowed_option(argv[1]) && !is_allowed_name(argv[1]))
            goto illegal_option;

        void *corresponding_var = hash_map_get(rt->variables, argv[1]);
        if (!corresponding_var) // var doesn't exist
            return 0;
        else // var exists
            var_hash_map_remove(rt->variables, (argv[1]));
    }

    else if (argc >= 3)
    {
        // variable name
        if (!strcmp("-v", argv[1]) || !strcmp("-fv", argv[1]))
        {
            for (int c = 2; c < argc; c++)
            {
                if (!is_allowed_name(argv[c]))
                {
                    warnx("unset: %s: bad variable name", argv[c]);
                    rt->no_new_status = true;
                    return 2;
                }

                void *corresponding_var = hash_map_get(rt->variables, argv[c]);
                if (!corresponding_var) // var doesn't exist
                    return 0;
                else // var exists
                    var_hash_map_remove(rt->variables, (argv[c]));
            }
        }

        // function name
        else if (!strcmp("-f", argv[1]) || !strcmp("-vf", argv[1]))
        {
            for (int c = 2; c < argc; c++)
            {
                if (!is_allowed_name(argv[c]))
                {
                    warnx("unset: %s: bad variable name", argv[c]);
                    rt->no_new_status = true;
                    return 2;
                }

                void *corresponding_func =
                    hash_map_func_get(rt->functions, argv[c]);
                if (!corresponding_func) // var doesn't exist
                    return 0;
                else // var exists
                    hash_map_func_remove(rt->functions, (argv[c]));
            }
        }

        // 'unset name1 name2 name3 ...'
        else if (is_allowed_name(argv[1]))
        {
            for (int c = 2; c < argc; c++)
            {
                if (!is_allowed_name(argv[c]))
                {
                    warnx("unset: %s: bad variable name", argv[c]);
                    rt->no_new_status = true;
                    return 2;
                }

                void *corresponding_var = hash_map_get(rt->variables, argv[c]);
                if (!corresponding_var) // var doesn't exist
                    return 0;
                else // var exists
                    var_hash_map_remove(rt->variables, (argv[c]));
            }
        }

        // 'unset -something_not_allowed ...'
        else
            goto illegal_option;
    }

    return 0;

illegal_option:
    warnx("unset: Illegal option %s", argv[1]);
    rt->no_new_status = true;
    return 2;
}
