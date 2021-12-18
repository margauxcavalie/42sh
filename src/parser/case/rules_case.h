#pragma once

#include <parser/parser.h>

/* rules */
enum parser_status parse_rule_case(struct ast_node **ast, struct lexer **lexer);
