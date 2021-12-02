#pragma once

#include <parser/parser.h>

/* rules */
enum parser_status parse_rule_command_list(struct ast_node **ast,
                                           struct lexer **lexer);

enum parser_status parse_rule_compound_list(struct ast_node **ast,
                                            struct lexer **lexer);
