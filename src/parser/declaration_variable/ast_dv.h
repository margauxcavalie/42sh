#pragma once

#include <parser/ast_node.h>

struct ast_dv
{
    struct ast_node base; // not a pointer
    char *key;
    char *value;
    struct ast_node *child;
};

struct ast_dv *ast_dv_init();
int ast_dv_exec(struct ast_node *ast, struct runtime *rt);
void ast_dv_print(struct ast_node *ast, struct print_context pc);
void ast_dv_free(struct ast_node *ast);

void ast_dv_set_child(struct ast_dv *ast, struct ast_node *child);
void ast_dv_set_key_value_pair(struct ast_dv *ast, char *data);
