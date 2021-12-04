#pragma once

#include <parser/ast_node.h>

struct ast_for
{
    struct ast_node base; // not a pointer
    struct vector *condition;
    struct ast_node *body;
    size_t advancement;
    char *var_name;
};

struct ast_for *ast_for_init();
int ast_for_exec(struct ast_node *ast, struct runtime *rt);
void ast_for_print(struct ast_node *ast, struct print_context pc);
void ast_for_free(struct ast_node *ast);

void ast_for_add_word(struct ast_for *ast, char *word);
void ast_for_set_body(struct ast_for *ast, struct ast_node *body);
void ast_for_set_var_name(struct ast_for *ast, char *name);
