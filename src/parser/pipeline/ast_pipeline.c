#include "ast_pipeline.h"

#include <err.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
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
static int ast_pipeline_exec_child(struct vector *v, struct runtime *rt)
{
    int last_return_code = 0;
    int pipefds[2];
    int save_stdin = dup(STDIN_FILENO);
    for (size_t i = 0; i < v->size - 1; i++)
    {
        pipe(pipefds);
        pid_t pid = fork();
        if (pid == 0)
        {
            dup2(pipefds[1], STDOUT_FILENO);
            exit(ast_node_exec(v->data[i], rt));
            // errx(1, "iuzerfiuzbzheizufebbizfeu");
        }
        dup2(pipefds[0], STDIN_FILENO);
        close(pipefds[1]);
        int wstatus;
        waitpid(pid, &wstatus, 0);
        last_return_code = WEXITSTATUS(wstatus);
    }
    last_return_code = ast_node_exec(v->data[v->size - 1], rt);
    dup2(save_stdin, STDIN_FILENO);
    close(pipefds[0]);
    close(save_stdin);
    return last_return_code;
}

int ast_pipeline_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_pipeline *ast_pipeline = (struct ast_pipeline *)ast;
    struct vector *v = ast_pipeline->ast_list;
    if (v->size == 0) // Vector is empty
        return 0;

    return ast_pipeline_exec_child(v, rt);
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
    return new_ast;
}

void ast_pipeline_add_ast(struct ast_pipeline *ast, struct ast_node *ast_elt)
{
    ast->ast_list = vector_append(ast->ast_list, ast_elt);
}
