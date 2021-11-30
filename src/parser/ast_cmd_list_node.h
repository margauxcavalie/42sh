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

enum parser_status parse_rule_command_list(struct ast_node **ast,
                                           struct lexer *lexer);
enum parser_status parse_rule_compound_list(struct ast_node **ast,
                                            struct lexer *lexer);
