#include "ast_subshell.h"

#include <err.h>
#include <expansion/expansion.h>
#include <stdio.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
void ast_subshell_free(struct ast_node *ast)
{
    struct ast_subshell *ast_subshell = (struct ast_subshell *)ast;
    ast_node_free(ast_subshell->body);
}

/**
 * @brief Prints the content of the AST
 */
void ast_subshell_print(struct ast_node *ast, struct print_context pc)
{
    // struct print_context new_pc = { pc.indent + 1 };

    struct ast_subshell *ast_subshell = (struct ast_subshell *)ast;
    ast_node_print_indent(pc.indent); // add indent
    printf("subshell { ");
    printf("\n");
    ast_node_print_indent(pc.indent);
    ast_node_print_rec(ast_subshell->body, pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("};");
}

/**
 * @brief Executes the body of the subshell
 */
int ast_subshell_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_subshell *ast_subshell = (struct ast_subshell *)ast;

    FILE *save_file = rt->file;

    int exit_code = 666;
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

        exit_code = ast_node_exec(ast_subshell->body, rt);
        exit(exit_code);
    }
    else // father
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if ((exit_code = WEXITSTATUS(wstatus)) == 127)
            errx(1, "unexpected error in subshell");

        // retore file here
        rt->file = save_file;
    }

    return exit_code;
}

/**
 * @brief Initializes an AST for. Its vector condition has a size 5
 */
struct ast_subshell *ast_subshell_init()
{
    struct ast_subshell *new_ast =
        xmalloc(sizeof(struct ast_subshell)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_SUBSHELL;
    base->node_free = &ast_subshell_free;
    base->node_print = &ast_subshell_print;
    base->node_exec = &ast_subshell_exec;

    new_ast->body = NULL;
    return new_ast;
}

void ast_subshell_set_body(struct ast_subshell *ast, struct ast_node *body)
{
    ast->body = body;
}
