#pragma once

#include "ast_node.h"
#include "parser.h"

/**
 * Always a leaf
 */
struct ast_simple_cmd_node
{
    struct ast_node base; // not a pointer
    struct vector *params;
};

enum parser_status parse_rule_simple_cmd(struct ast_node **ast,
                                         struct lexer *lexer);

void ast_simple_cmd_free(struct ast_node *ast);
void ast_simple_cmd_print(struct ast_node *ast, struct print_context pc);
int ast_simple_cmd_exec(struct ast_node *ast);
struct ast_simple_cmd_node *ast_simple_cmd_init(void);
struct ast_simple_cmd_node *
ast_simple_cmd_add_param(struct ast_simple_cmd_node *ast, char *param);
