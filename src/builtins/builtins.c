#include <builtins/builtins.h>

int exec_builtin(struct ast_simple_cmd *ast, bool *is_builtin)
{
    struct matching_builtin our_builtins[] = { { "echo", &builtin_echo },
                                               { "cd", &builtin_cd } };
    size_t size = sizeof(our_builtins) / sizeof(struct matching_builtin);

    for (size_t i = 0; i < size; i++)
    {
        if (!strcmp(ast->params->data[0], our_builtins[i].cmd))
        {
            size_t params_size = ast->params->size;
            char **params_cast = zalloc(sizeof(char *) * (params_size + 1));
            for (size_t i = 0; i < params_size; i++)
            {
                params_cast[i] = ast->params->data[i];
            }
            *is_builtin = true;
            int result = (*(our_builtins[i].exec))(params_size, params_cast);
            free(params_cast);
            return result;
        }
    }
    *is_builtin = false;
    return 0;
}
