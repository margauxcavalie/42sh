#pragma once

#include <parser/ast_node.h>

struct ast_pipeline
{
    struct ast_node base; // not a pointer
    struct vector *ast_list;
};

struct ast_pipeline *ast_pipeline_init(void);
int ast_pipeline_exec(struct ast_node *ast, struct runtime *rt);
void ast_pipeline_print(struct ast_node *ast, struct print_context pc);
void ast_pipeline_free(struct ast_node *ast);
void ast_pipeline_add_ast(struct ast_pipeline *ast, struct ast_node *ast_elt);
