#pragma once

#include <parser/ast_node.h>

struct ast_while
{
    struct ast_node base; // not a pointer
    struct ast_node *condition;
    struct ast_node *body;
};

struct ast_while *ast_while_init();
int ast_while_exec(struct ast_node *ast);
void ast_while_print(struct ast_node *ast, struct print_context pc);
void ast_while_free(struct ast_node *ast);

void ast_while_set_condition(struct ast_while *ast, struct ast_node *condition);
void ast_while_set_body(struct ast_while *ast, struct ast_node *body);
