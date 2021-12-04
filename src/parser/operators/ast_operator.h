#pragma once

#include <parser/ast_node.h>
#include <lexer/operators.h>

struct ast_operator
{
    struct ast_node base;
    enum op_type type;
    struct ast_node *left;
    struct ast_node *right;
};

struct ast_operator *ast_operator_init(enum op_type type);
int ast_operator_exec(struct ast_node *ast, struct runtime *rt);
void ast_operator_print(struct ast_node *ast, struct print_context pc);
void ast_operator_free(struct ast_node *ast);