#pragma once

#include <parser/ast_node.h>

struct ast_if
{
    struct ast_node base; // not a pointer
    struct ast_node *condition;
    struct ast_node *if_body;
    struct ast_node *else_body;
};

struct ast_if *ast_if_init();
int ast_if_exec(struct ast_node *ast, struct runtime *rt);
void ast_if_print(struct ast_node *ast, struct print_context pc);
void ast_if_free(struct ast_node *ast);

void ast_if_set_condition(struct ast_if *ast, struct ast_node *condition);
void ast_if_set_body(struct ast_if *ast, struct ast_node *body);
void ast_if_set_else(struct ast_if *ast, struct ast_node *else_body);
