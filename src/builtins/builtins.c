#include <builtins/builtins.h>
#include <stdio.h>

int exec_builtin(struct vector *argv, bool *is_builtin)
{
    struct matching_builtin our_builtins[] = { { "echo", &builtin_echo } };
    size_t size = sizeof(our_builtins) / sizeof(struct matching_builtin);

    for (size_t i = 0; i < size; i++)
    {
        if (!strcmp(argv->data[0], our_builtins[i].cmd))
        {
            size_t params_size = argv->size;
            char **params_cast = zalloc(sizeof(char *) * (params_size + 1));
            for (size_t i = 0; i < params_size; i++)
            {
                params_cast[i] = argv->data[i];
            }
            *is_builtin = true;
            int result = (*(our_builtins[i].exec))(params_size, params_cast);
            fflush(stdout);
            free(params_cast);
            return result;
        }
    }
    *is_builtin = false;
    return 0;
}
