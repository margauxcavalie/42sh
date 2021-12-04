#pragma once

#include <parser/ast_node.h>
#include <runtime.h>

struct ast_cmd_list
{
    struct ast_node base; // not a pointer
    struct vector *ast_list;
};

/* structure */
struct ast_cmd_list *ast_cmd_list_init(void);
int ast_cmd_list_exec(struct ast_node *ast, struct runtime *rt);
void ast_cmd_list_print(struct ast_node *ast, struct print_context pc);
void ast_cmd_list_free(struct ast_node *ast);
void ast_cmd_list_add_ast(struct ast_cmd_list *ast, struct ast_node *ast_elt);
