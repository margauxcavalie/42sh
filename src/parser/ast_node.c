#include "ast_node.h"

#include <stdio.h>
#include <utils/alloc.h>

/*
 *   @brief free recursively the entire ast
 *
 */
void ast_node_free(void *ptr)
{
    struct ast_node *ast = ptr;
    if (ast == NULL)
        return;
    if (ast->node_free != NULL)
        (*(ast->node_free))(ast);
    free(ast);
}

/*
 *   @brief free recursively the entire ast and set the root as NULL
 *
 */
void ast_node_free_detach(struct ast_node **ast)
{
    if (*ast == NULL)
        return;
    if ((*ast)->node_free != NULL)
        (*((*ast)->node_free))(*ast);
    free(*ast);
    *ast = NULL;
}

void ast_node_print_rec(void *ptr, struct print_context pc)
{
    struct ast_node *ast = ptr;
    if (ast == NULL)
        return;
    (*(ast->node_print))(ast, pc);
}

void ast_node_print_indent(int indent)
{
    printf("%*s", indent * 4, "");
}

// Do not use that
void ast_node_print(void *ptr)
{
    struct print_context pc = { 0 };
    ast_node_print_rec(ptr, pc);
    printf("\n");
}

int ast_node_exec(void *ptr, struct runtime *rt)
{
    if (ptr == NULL)
        return 0;
    struct ast_node *ast = ptr;

    if (rt->loops_to_break != 0 || rt->encountered_continue)
        return 0;

    int status = (*(ast->node_exec))(ast, rt);
    runtime_set_status(rt, status);
    return status;
}
