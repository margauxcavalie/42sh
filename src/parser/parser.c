#include "parser.h"

#include <parser/cmd_list/rules_cmd_list.h>
#include <parser/for/rules_for.h>
#include <parser/function/rule_function.h>
#include <parser/if/rules_if.h>
#include <parser/pipeline/rule_pipeline.h>
#include <parser/redir/ast_redir.h>
#include <parser/redir/rules_redir.h>
#include <parser/simple_cmd/rules_simple_cmd.h>
#include <parser/while_until/rules_while_until.h>

enum parser_status get_error_status(struct token *tok)
{
    if (tok->type == TOKEN_EOF)
        return PARSER_UNEXPECTED_EOF;
    return PARSER_UNEXPECTED_TOKEN;
}

static enum parser_status handle_parse_error(enum parser_status status,
                                             struct ast_node **ast)
{
    ast_node_free(*ast);
    *ast = NULL;
    return status;
}

/**
 * @brief temporary version
 * shell_command: '{' compound_list '}'
 *          | rule_for
 *          | rule_while
 *          | rule_until
 *          | rule_if
 *
 * @return enum parser_status
 */
enum parser_status parse_rule_shell_cmd(struct ast_node **ast,
                                        struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    struct token *tok = lexer_peek(*lexer);
    if (is_op(tok, OP_LBRACE)) // '{'
    {
        token_free(lexer_pop(*lexer));
        status = parse_rule_compound_list(ast, lexer);
        if (status != PARSER_OK)
            goto error;
        struct token *tok = lexer_peek(*lexer);
        if (is_op(tok, OP_RBRACE)) // '}'
        {
            token_free(lexer_pop(*lexer));
            lexer_free_without_pretokens(saved_lexer);
            return PARSER_OK;
        }
        status = get_error_status(tok);
        goto error;
    }

    status = parse_rule_for(ast, lexer);
    if (status == PARSER_UNEXPECTED_TOKEN)
    {
        status = parse_rule_while(ast, lexer);
        if (status == PARSER_UNEXPECTED_TOKEN)
        {
            status = parse_rule_until(ast, lexer);
            if (status == PARSER_UNEXPECTED_TOKEN)
            {
                status = parse_rule_if(ast, lexer);
                if (status == PARSER_UNEXPECTED_TOKEN)
                    goto error;
            }
        }
    }

    lexer_free_without_pretokens(saved_lexer);
    return status;
error:
    restore_lexer(lexer, saved_lexer);
    return status;
}

/**
 * @brief temporary version
 * command: simple_command
 *      |   shell_command (redirection)*
 *
 * @return enum parser_status
 */
enum parser_status parse_rule_cmd(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status = parse_rule_simple_cmd(ast, lexer);
    if (status == PARSER_UNEXPECTED_TOKEN)
    {
        status = parse_rule_shell_cmd(ast, lexer);
        if (status != PARSER_OK)
            goto error;
        struct ast_node *ast_shell_cmd = *ast;

        struct ast_node **saved_ast = ast;

        struct ast_node *ast_cur = *ast;
        ast = &ast_cur;

        struct ast_node *last_parent_ast = NULL;

        while (true)
        {
            status = parse_rule_redirection(ast, lexer);
            if (status == PARSER_UNEXPECTED_TOKEN)
                break;
            if (last_parent_ast == NULL)
                *saved_ast = *ast;
            else
                ast_redir_set_child((struct ast_redir *)last_parent_ast, *ast);
            last_parent_ast = *ast;
            *ast = ast_shell_cmd;
        }
    }
    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return status;
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
    return handle_parse_error(status, ast);
}
