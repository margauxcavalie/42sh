#include "parser.h"

#include <parser/cmd_list/rules_cmd_list.h>
#include <parser/for/rules_for.h>
#include <parser/if/rules_if.h>
#include <parser/pipeline/rule_pipeline.h>
#include <parser/simple_cmd/rules_simple_cmd.h>
#include <parser/while_until/rules_while_until.h>
#include <stdio.h> // DELETE

static enum parser_status handle_parse_error(enum parser_status status,
                                             struct ast_node **ast)
{
    warnx("unexpected token");
    ast_node_free(*ast);
    *ast = NULL;
    return status;
}

/**
 * @brief temporary version
 * shell_command: rule_if
 *          | rule_while
 *          | rule_until
 *          | rule_for
 *
 * @return enum parser_status
 */
enum parser_status parse_rule_shell_cmd(struct ast_node **ast,
                                        struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status = parse_rule_if(ast, lexer);
    if (status != PARSER_OK)
    {
        status = parse_rule_while(ast, lexer);
        if (status != PARSER_OK)
        {
            status = parse_rule_until(ast, lexer);
            if (status != PARSER_OK)
            {
                status = parse_rule_for(ast, lexer);
                if (status != PARSER_OK)
                    goto error;
            }
        }
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief temporary version
 * command: simple_command
 *      |   shell_command
 *
 * @return enum parser_status
 */
enum parser_status parse_rule_cmd(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status = parse_rule_simple_cmd(ast, lexer);
    if (status != PARSER_OK)
    {
        status = parse_rule_shell_cmd(ast, lexer);
        if (status != PARSER_OK)
            goto error;
    }
    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief temporary version
 * input: list '\n'
 *      | list EOF
 *      | '\n'
 *      | EOF
 * @return enum parser_status
 */
enum parser_status parse(struct ast_node **ast, struct lexer **lexer)
{
    enum parser_status status =
        parse_rule_command_list(ast, lexer); // simple command
    if (status == PARSER_OK) // not a simple command
    {
        struct token *tok = lexer_peek(*lexer);
        if ((is_op(tok, OP_LINEFEED)) || tok->type == TOKEN_EOF) // '\n' | EOF
            return PARSER_OK;
        return handle_parse_error(PARSER_UNEXPECTED_TOKEN, ast);
    }

    struct token *tok = lexer_peek(*lexer);
    if ((is_op(tok, OP_LINEFEED)) || tok->type == TOKEN_EOF) // '\n' | EOF
        return PARSER_OK;

    // ERROR HERE
    return handle_parse_error(PARSER_UNEXPECTED_TOKEN, ast);
}
