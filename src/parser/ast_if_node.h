#pragma once

#include "ast_node.h"
#include "parser.h"

/**
 *
 */
struct ast_if_node
{
    struct ast_node base; // not a pointer
    struct ast_node *condition;
    struct ast_node *if_body;
    struct ast_node *else_body;
};

enum parser_status parse_rule_if(struct ast_node **ast,
                                           struct lexer *lexer);
