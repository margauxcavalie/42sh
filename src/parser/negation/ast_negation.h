#pragma once

#include <parser/ast_node.h>

struct ast_negation
{
    struct ast_node base;
    struct ast_node *child;
};

struct ast_negation *ast_negation_init(void);
int ast_negation_exec(struct ast_node *ast, struct runtime *rt);
void ast_negation_print(struct ast_node *ast, struct print_context pc);
void ast_negation_free(struct ast_node *ast);