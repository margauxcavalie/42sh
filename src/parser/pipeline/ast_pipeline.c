#include "ast_pipeline.h"

#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <utils/alloc.h>
#include <vector/vector.h>

void ast_pipeline_free(struct ast_node *ast)
{
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;
    vector_apply_on_elts(ast_pipeline->ast_list, &ast_node_free);
    vector_destroy(ast_pipeline->ast_list);
}

void ast_pipeline_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;
    struct vector *v = ast_pipeline->ast_list;
    if (v->size == 0) // Vector is empty
        return;

    for (size_t i = 0; i < v->size - 1; i++)
    {
        ast_node_print_rec(v->data[i], pc);
        printf("|\n");
    }

    ast_node_print_rec(v->data[v->size - 1], pc);
}

/**
 * @brief test
 *
 * @param pipefds the pipe where the program will write/read to
 * @param v vector of command
 * @return integer (0 if successful, -1 otherwise)
 */
static int ast_pipeline_exec_child(int *pipefds, struct vector *v)
{
    int last_return_code = 0;
    // int saved_stdout = dup(1);
    // int saved_stdin = dup(0);
    for (size_t i = 0; i < v->size; i++)
    {
        if (i == 0) // i is the first command
        {
            // redirect to pipe STDOUT
            int saved_stdout = dup(1);
            if (dup2(pipefds[1], STDOUT_FILENO) == -1)
                return 127;
            ast_node_exec(v->data[i]);
            dup2(saved_stdout, 1);
            close(saved_stdout);
        }
        else if (i == v->size - 1) // i is the last command
        {
            // take from pipe STDIN
            int saved_stdin = dup(0);
            if (dup2(pipefds[0], STDIN_FILENO) == -1)
                return 127;
            last_return_code = ast_node_exec(v->data[i]);
            dup2(saved_stdin, 0);
            close(saved_stdin);
        }
        else
        {
            // take from pipe STDIN
            if (dup2(pipefds[0], STDIN_FILENO) == -1)
                return 127;
            // redirect to pipe STDOUT
            if (dup2(pipefds[1], STDOUT_FILENO) == -1)
                return 127;
            ast_node_exec(v->data[i]);
        }
    }

    return last_return_code;
}

int ast_pipeline_exec(struct ast_node *ast)
{
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;
    struct vector *v = ast_pipeline->ast_list;
    if (v->size == 0) // Vector is empty
        return 0;

    if (!v || v->size == 0) // Vector is empty or non-existent
    { // usually impossible since a command list must have at least 1 command
        return 1;
    }

    return ast_pipeline_exec_child(ast_pipeline->pipefds, v);
}

struct ast_pipeline *ast_pipeline_init(void)
{
    struct ast_pipeline *new_ast =
        xmalloc(sizeof(struct ast_pipeline)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_PIPELINE;
    base->node_free = &ast_pipeline_free;
    base->node_print = &ast_pipeline_print;
    base->node_exec = &ast_pipeline_exec;

    new_ast->ast_list = vector_init(5);
    pipe(new_ast->pipefds);
    return new_ast;
}

void ast_pipeline_add_ast(struct ast_pipeline *ast, struct ast_node *ast_elt)
{
    ast->ast_list = vector_append(ast->ast_list, ast_elt);
}
