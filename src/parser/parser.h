#pragma once

#include <err.h>

#include "ast_node.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNTERMINATED_QUOTES,
    PARSER_UNEXPECTED_EOF,
    PARSER_UNEXPECTED_TOKEN,
};

enum parser_status get_error_status(struct token *tok);

enum parser_status parse(struct ast_node **ast, struct lexer **lexer);
enum parser_status parse_rule_cmd(struct ast_node **ast, struct lexer **lexer);
enum parser_status parse_rule_shell_cmd(struct ast_node **ast,
                                        struct lexer **lexer);
