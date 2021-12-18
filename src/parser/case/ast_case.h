#pragma once

#include <parser/ast_node.h>

struct ast_case
{
    struct ast_node base; // not a pointer
    struct vector *patterns;
    struct vector *bodies;
    char *reference;
};

struct ast_case *ast_case_init();
int ast_case_exec(struct ast_node *ast, struct runtime *rt);
void ast_case_print(struct ast_node *ast, struct print_context pc);
void ast_case_free(struct ast_node *ast);

void ast_case_add_pattern(struct ast_case *ast, struct vector *pattern);
void ast_case_add_body(struct ast_case *ast, struct ast_node *body);
void ast_case_set_reference(struct ast_case *ast, char *name);
