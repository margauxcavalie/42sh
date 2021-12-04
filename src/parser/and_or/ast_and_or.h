#pragma once

#include <parser/ast_node.h>
#include <lexer/operators.h>

struct ast_and_or
{
    struct ast_node base;
    enum op_type type;
    struct ast_node *left;
    struct ast_node *right;
};

struct ast_and_or *ast_and_or_init(enum op_type type);
int ast_and_or_exec(struct ast_node *ast, struct runtime *rt);
void ast_and_or_print(struct ast_node *ast, struct print_context pc);
void ast_and_or_free(struct ast_node *ast);