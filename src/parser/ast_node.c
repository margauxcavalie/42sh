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
    (*(ast->node_free))(ast);
    free(ast);
}

void ast_node_print_rec(void *ptr)
{
    struct ast_node *ast = ptr;
    (*(ast->node_print))(ast);
}

// Do not use that
void ast_node_print(void *ptr)
{
    ast_node_print_rec(ptr);
    printf("\n");
}