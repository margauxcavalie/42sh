#pragma once

#include <parser/ast_node.h>

struct ast_function
{
    struct ast_node base;
    struct ast_node *child;
    char *name;
};

struct ast_function *ast_function_init();
int ast_function_exec(struct ast_node *ast, struct runtime *rt);
void ast_function_print(struct ast_node *ast, struct print_context pc);
void ast_function_free(struct ast_node *ast);