#pragma once

#include <parser/parser.h>

enum parser_status parse_rule_do_group(struct ast_node **ast,
                                       struct lexer **lexer);
enum parser_status parse_rule_while(struct ast_node **ast,
                                    struct lexer **lexer);
enum parser_status parse_rule_until(struct ast_node **ast,
                                    struct lexer **lexer);
