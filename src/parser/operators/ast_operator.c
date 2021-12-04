#include "ast_operator.h"

#include <stdio.h>
#include <utils/alloc.h>

struct ast_operator *ast_operator_init(enum op_type type)
{
    struct ast_operator *new_ast =
        xmalloc(sizeof(struct ast_operator)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_NEG;
    base->node_free = &ast_operator_free;
    base->node_print = &ast_operator_print;
    base->node_exec = &ast_operator_exec;

    new_ast->type = type;
    new_ast->left = NULL;
    new_ast->right = NULL;
    return new_ast;
}

int ast_operator_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_operator *ast_operator = (struct ast_operator *)ast;
    if (ast_operator->type == OP_AND)
    {
        int rc_and = ast_node_exec(ast_operator->left, rt); 
        if (!rc_and)
            return ast_node_exec(ast_operator->right, rt);
        else
            return rc_and;
    }
    else
    {
        int rc_or = ast_node_exec(ast_operator->left, rt);
        if (rc_or)
            return ast_node_exec(ast_operator->right, rt);
        else
            return rc_or;
    }
}

void ast_operator_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_operator *ast_operator = (struct ast_operator *)ast;
    ast_node_print_rec(ast_operator->left, pc);
    if (ast_operator->type == OP_OR)
        printf(" || ");
    else
        printf(" && ");
    ast_node_print_rec(ast_operator->right, pc);
}

void ast_operator_free(struct ast_node *ast)
{
    struct ast_operator *ast_operator = (struct ast_operator *)ast;
    ast_node_free(ast_operator->left);
    ast_node_free(ast_operator->right);
}
