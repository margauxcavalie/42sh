#pragma once

#include "ast_node.h"
#include "parser.h"

/**
 *
 */
struct ast_cmd_list_node
{
    struct ast_node base; // not a pointer
    struct vector *ast_list;
};

/* structure */
struct ast_cmd_list_node *ast_cmd_list_init(void);
int ast_cmd_list_exec(struct ast_node *ast);
void ast_cmd_list_print(struct ast_node *ast, struct print_context pc);
void ast_cmd_list_free(struct ast_node *ast);
void ast_cmd_list_add_ast(struct ast_cmd_list_node *ast,
                                 struct ast_node *ast_elt);


/* rules */
enum parser_status parse_rule_command_list(struct ast_node **ast,
                                           struct lexer *lexer);
enum parser_status parse_rule_compound_list(struct ast_node **ast,
                                            struct lexer *lexer);
