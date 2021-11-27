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

enum parser_status parse_rule_simple_command(struct ast_node **ast,
                                             struct lexer *lexer);
