#include "rules_cmd_list.h"

#include <lexer/lexer.h>
#include <parser/simple_cmd/ast_simple_cmd.h>
#include <stdlib.h>
#include <vector/vector.h>

#include "ast_cmd_list.h"

/**
 * @brief (temporary version)
 * list: command (';' command)* [';']
 * @return
 */
enum parser_status parse_rule_command_list(struct ast_node **ast,
                                           struct lexer *lexer)
{
    struct ast_cmd_list *ast_list =
        ast_cmd_list_init(); // Create an empy AST cmdlist
    *ast = (struct ast_node *)ast_list; // attach ast_list to ast

    struct ast_node *ast_child = NULL;
    enum parser_status status = parse_rule_cmd(&ast_child, lexer); // command
    ast_cmd_list_add_ast(ast_list, ast_child);
    if (status != PARSER_OK)
        goto error;

    while (is_op(lexer_peek(lexer), OP_SEMICOLON)) // (';' command)* [';']
    {
        struct token *tok = lexer_pop(lexer);
        token_free(tok);
        enum parser_status status = parse_rule_cmd(&ast_child, lexer);
        if (status != PARSER_OK)
            break;
        ast_cmd_list_add_ast(ast_list, ast_child);
    }

    return PARSER_OK;

error:
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief
 * compound_separator: (';'|'\n') ('\n')*
 * @return
 */
static enum parser_status parse_rule_compound_separator(struct lexer *lexer)
{
    // check if ';' or '\n'
    if (!(is_op(lexer_peek(lexer), OP_SEMICOLON))
        && (!(is_op(lexer_peek(lexer), OP_LINEFEED))))
    {
        return PARSER_UNEXPECTED_TOKEN;
    }

    token_free(lexer_pop(lexer));
    while (is_op(lexer_peek(lexer), OP_LINEFEED)) // while '\n'
    {
        token_free(lexer_pop(lexer));
    }

    return PARSER_OK;
}

/**
 * @brief (temporary version)
 * compound_list: ('\n')* command (compound_sep command)* [compound_sep]
 * @return
 */
enum parser_status parse_rule_compound_list(struct ast_node **ast,
                                            struct lexer *lexer)
{
    while (is_op(lexer_peek(lexer), OP_LINEFEED)) // ('\n')*
    {
        token_free(lexer_pop(lexer));
    }
    struct ast_cmd_list *ast_list =
        ast_cmd_list_init(); // Create an empy AST cmdlist
    *ast = (struct ast_node *)ast_list; // attach ast_list to ast

    struct ast_node *ast_child = NULL;
    enum parser_status status = parse_rule_cmd(&ast_child, lexer); // command
    ast_cmd_list_add_ast(ast_list, ast_child);
    if (status != PARSER_OK)
        goto error;

    // (compound_separator command)* [compound_separator]
    while (parse_rule_compound_separator(lexer) == PARSER_OK)
    {
        enum parser_status status = parse_rule_cmd(&ast_child, lexer);
        if (status != PARSER_OK)
            break;
        ast_cmd_list_add_ast(ast_list, ast_child);
    }
    return PARSER_OK;
error:
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}
