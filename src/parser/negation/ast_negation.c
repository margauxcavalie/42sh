#include "ast_negation.h"

#include <stdio.h>
#include <utils/alloc.h>

void ast_negation_free(struct ast_node *ast)
{
    struct ast_negation *ast_negation = (struct ast_negation *)ast;
    ast_node_free(ast_negation->child);
}

void ast_negation_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_negation *ast_negation = (struct ast_negation *)ast;
    printf("!");
    ast_node_print_rec(ast_negation->child, pc);
}

int ast_negation_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_negation *ast_negation = (struct ast_negation *)ast;
    int rc = ast_node_exec(ast_negation->child, rt);
    return (rc == 0) ? 1 : 0;
}

struct ast_negation *ast_negation_init(void)
{
    struct ast_negation *new_ast =
        xmalloc(sizeof(struct ast_negation)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_NEG;
    base->node_free = &ast_negation_free;
    base->node_print = &ast_negation_print;
    base->node_exec = &ast_negation_exec;

    new_ast->child = NULL;
    return new_ast;
}