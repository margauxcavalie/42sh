#pragma once

#include <parser/ast_node.h>

struct ast_while_until
{
    struct ast_node base; // not a pointer
    struct ast_node *condition;
    struct ast_node *body;
};

// specific
struct ast_while_until *ast_while_init();
struct ast_while_until *ast_until_init();
int ast_while_exec(struct ast_node *ast);
int ast_until_exec(struct ast_node *ast);
void ast_while_print(struct ast_node *ast, struct print_context pc);
void ast_until_print(struct ast_node *ast, struct print_context pc);

// generic for both while and until
void ast_while_until_free(struct ast_node *ast);
void ast_while_until_set_condition(struct ast_while_until *ast,
                                   struct ast_node *condition);
void ast_while_until_set_body(struct ast_while_until *ast,
                              struct ast_node *body);
