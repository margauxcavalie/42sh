#pragma once

#include <parser/parser.h>

enum parser_status parse_rule_funcdec(struct ast_node **ast,
                                      struct lexer **lexer);