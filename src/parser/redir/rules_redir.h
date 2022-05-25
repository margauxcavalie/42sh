#pragma once

#include <parser/parser.h>

/* rules */
enum parser_status parse_rule_redirection(struct ast_node **ast,
                                          struct lexer **lexer);
