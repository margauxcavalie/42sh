#include "ast_and_or.h"

#include <stdio.h>
#include <utils/alloc.h>

struct ast_and_or *ast_and_or_init(enum op_type type)
{
    struct ast_and_or *new_ast =
        xmalloc(sizeof(struct ast_and_or)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_NEG;
    base->node_free = &ast_and_or_free;
    base->node_print = &ast_and_or_print;
    base->node_exec = &ast_and_or_exec;

    new_ast->type = type;
    new_ast->left = NULL;
    new_ast->right = NULL;
    return new_ast;
}

int ast_and_or_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    if (ast_and_or->type == OP_AND)
    {
        int rc_and = ast_node_exec(ast_and_or->left, rt); 
        if (!rc_and)
            return ast_node_exec(ast_and_or->right, rt);
        else
            return rc_and;
    }
    else
    {
        int rc_or = ast_node_exec(ast_and_or->left, rt);
        if (rc_or)
            return ast_node_exec(ast_and_or->right, rt);
        else
            return rc_or;
    }
}

void ast_and_or_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    ast_node_print_rec(ast_and_or->left, pc);
    if (ast_and_or->type == OP_OR)
        printf(" || ");
    else
        printf(" && ");
    ast_node_print_rec(ast_and_or->right, pc);
}

void ast_and_or_free(struct ast_node *ast)
{
    struct ast_and_or *ast_and_or = (struct ast_and_or *)ast;
    ast_node_free(ast_and_or->left);
    ast_node_free(ast_and_or->right);
}
