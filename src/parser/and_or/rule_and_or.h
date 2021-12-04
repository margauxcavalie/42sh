#pragma once

#include <parser/parser.h>

enum parser_status parse_rule_and_or(struct ast_node **ast,
                                     struct lexer **lexer);