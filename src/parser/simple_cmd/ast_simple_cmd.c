#include "ast_simple_cmd.h"

#include <builtins/builtins.h>
#include <err.h>
#include <expansion/expansion.h>
#include <hash_map_function/hash_map_function.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utils/alloc.h>

/**
 * @brief Frees all the AST contains
 */
void ast_simple_cmd_free(struct ast_node *ast)
{
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;
    vector_apply_on_elts(ast_simple_cmd->params, &free);
    vector_destroy(ast_simple_cmd->params);
}

/**
 * @brief Prints the content of an AST simple_cmd. Isn't recursive.
 */
void ast_simple_cmd_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;
    struct vector *v = ast_simple_cmd->params;
    if (v->size == 0) // Vector is empty or non-existent
        return;

    ast_node_print_indent(pc.indent);

    for (size_t i = 0; i < v->size - 1; i++)
    {
        char *str = v->data[i];
        printf("%s ", str);
    }

    char *str = v->data[v->size - 1];
    printf("%s", str);
}

static struct vector *expands_simple_cmd_argv(struct ast_simple_cmd *ast,
                                              struct runtime *rt)
{
    struct vector *new = vector_init(ast->params->size);
    for (size_t i = 0; i < ast->params->size; i++)
    {
        struct vector *expanded_word = expands_word(ast->params->data[i], rt);
        if (expanded_word == NULL)
        {
            warnx("Expansion: unmacthed quotes (tmp)");
            vector_apply_on_elts(new, &free);
            vector_destroy(new);
            return NULL;
        }
        for (size_t j = 0; j < expanded_word->size; j++)
        {
            vector_append(new, expanded_word->data[j]);
        }
        vector_destroy(expanded_word); // do not destroy items
    }
    return new;
}

int ast_simple_cmd_exec(struct ast_node *ast, struct runtime *rt)
{
    rt->last_status = rt->last_status + 1 - 1; // TMP
    struct ast_simple_cmd *ast_simple_cmd = (struct ast_simple_cmd *)ast;

    // If there is nothing in the AST, do nothing
    if (ast_simple_cmd->params->size == 0)
        return 0;

    // expands command arguments
    struct vector *args_expended = expands_simple_cmd_argv(ast_simple_cmd, rt);
    if (args_expended == NULL)
    {
        return 2;
    }
    // Check if it's a function name
    struct ast_node *ast_function =
        hash_map_func_get(rt->functions, (char *)args_expended->data[0]);
    if (ast_function != NULL)
    {
        int status = ast_node_exec(ast_function, rt);
        vector_apply_on_elts(args_expended, &free);
        vector_destroy(args_expended);
        return status;
    }

    // Check if the command is a builtin
    bool is_builtin = false;
    int status = exec_builtin(args_expended, &is_builtin, rt);
    if (is_builtin)
    {
        // free expanded words vector
        vector_apply_on_elts(args_expended, &free);
        vector_destroy(args_expended);
        return status;
    }

    FILE *save_file = rt->file;

    pid_t pid = fork();
    if (pid == -1)
        return 1;
    if (pid == 0) // child
    {
        // close file if there is one
        if (rt->file)
        {
            fclose(rt->file);
            rt->file = NULL;
        }

        size_t params_size = args_expended->size;
        char **params_cast = zalloc(sizeof(char *) * (params_size + 1));
        for (size_t i = 0; i < params_size; i++)
        {
            params_cast[i] = args_expended->data[i];
        }

        int e = execvp(params_cast[0], params_cast);

        if (e == -1)
            warnx("command not found: %s", params_cast[0]);
        exit(127);
    }
    else // father
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);

        // retore file here
        rt->file = save_file;

        // free expanded words vector
        vector_apply_on_elts(args_expended, &free);
        vector_destroy(args_expended);
        return WEXITSTATUS(wstatus);
    }
}

/**
 * @brief Initializes a simple_command AST. Its vector has a size 5
 */
struct ast_simple_cmd *ast_simple_cmd_init(void)
{
    struct ast_simple_cmd *new_ast =
        xmalloc(sizeof(struct ast_simple_cmd)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_SIMPLE_CMD;
    base->node_free = &ast_simple_cmd_free;
    base->node_print = &ast_simple_cmd_print;
    base->node_exec = &ast_simple_cmd_exec;

    new_ast->params = vector_init(5);
    return new_ast;
}

/**
 * @brief Adds a new parameter to the ast. Mallocs a copy of the new parameter,
 * but does not free the old one.
 * @param param needs to have a '\\0' at the end
 */
struct ast_simple_cmd *ast_simple_cmd_add_param(struct ast_simple_cmd *ast,
                                                char *param)
{
    size_t len = strlen(param);
    // new allocation to avoid double free
    char *tmp = zalloc(sizeof(char) * (len + 1));
    strncpy(tmp, param, len);
    ast->params = vector_append(ast->params, tmp);
    return ast;
}
