#include "parser.h"

#include <parser/ast_cmd_list_node.h>

enum parser_status handle_parse_error(enum parser_status status,
                                      struct ast_node **ast)
{
    warnx("unexpected token");
    ast_node_free(*ast);
    *ast = NULL;
    return status;
}

/**
 * @brief temporary version
 * input: simple_command '\n'
 *      | simple_command EOF
 *      | '\n'
 *      | EOF
 * @return enum parser_status
 */
enum parser_status parse(struct ast_node **ast, struct lexer *lexer)
{
    enum parser_status status =
        parse_rule_command_list(ast, lexer); // simple command
    if (status != PARSER_OK) // not a simple command
    {
        struct token *tok = lexer_peek(lexer);
        if ((is_op(tok, OP_LINEFEED)) || tok->type == TOKEN_EOF) // '\n' | EOF
        {
            return PARSER_OK;
        }
        return handle_parse_error(status, ast);
    }
    struct token *tok = lexer_peek(lexer);
    if ((is_op(tok, OP_LINEFEED)) || tok->type == TOKEN_EOF) // '\n' | EOF
        return PARSER_OK;

    // ERROR HERE
    return handle_parse_error(PARSER_UNEXPECTED_TOKEN, ast);
}