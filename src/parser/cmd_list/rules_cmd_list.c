#include "rules_cmd_list.h"

#include <lexer/lexer.h>
#include <parser/and_or/ast_and_or.h>
#include <parser/and_or/rule_and_or.h>
#include <parser/simple_cmd/ast_simple_cmd.h>
#include <stdlib.h>
#include <vector/vector.h>

#include "ast_cmd_list.h"

/**
 * @brief temporary
 * list: and_or (';' and_or)* [';']
 * @return
 */
enum parser_status parse_rule_command_list(struct ast_node **ast,
                                           struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    struct ast_cmd_list *ast_list =
        ast_cmd_list_init(); // Create an empy AST cmdlist
    *ast = (struct ast_node *)ast_list; // attach ast_list to ast

    struct ast_node *ast_child = NULL;
    enum parser_status status = parse_rule_and_or(&ast_child, lexer); // command
    ast_cmd_list_add_ast(ast_list, ast_child);
    if (status != PARSER_OK)
        goto error;

    while (is_op(lexer_peek(*lexer), OP_SEMICOLON)) // (';' command)* [';']
    {
        struct token *tok = lexer_pop(*lexer);
        token_free(tok);
        status = parse_rule_and_or(&ast_child, lexer);
        if (status != PARSER_OK)
            break;
        ast_cmd_list_add_ast(ast_list, ast_child);
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return status;
}

/**
 * @brief
 * compound_separator: (';'|'\n') ('\n')*
 * @return
 */
static enum parser_status parse_rule_compound_separator(struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    // check if ';' or '\n'
    if (!(is_op(lexer_peek(*lexer), OP_SEMICOLON))
        && (!(is_op(lexer_peek(*lexer), OP_LINEFEED))))
    {
        status = get_error_status(lexer_peek(*lexer));
        goto error;
    }

    token_free(lexer_pop(*lexer));
    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // while '\n'
    {
        token_free(lexer_pop(*lexer));
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return status;
}

/**
 * @brief (temporary version)
 * compound_list: ('\n')* and_or (compound_sep and_or)* [compound_sep]
 * @return
 */
enum parser_status parse_rule_compound_list(struct ast_node **ast,
                                            struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
    {
        token_free(lexer_pop(*lexer));
    }
    struct ast_cmd_list *ast_list =
        ast_cmd_list_init(); // Create an empy AST cmdlist
    *ast = (struct ast_node *)ast_list; // attach ast_list to ast

    struct ast_node *ast_child = NULL;
    status = parse_rule_and_or(&ast_child, lexer); // command
    ast_cmd_list_add_ast(ast_list, ast_child);
    if (status != PARSER_OK)
        goto error;

    // (compound_separator command)* [compound_separator]
    while (parse_rule_compound_separator(lexer) == PARSER_OK)
    {
        status = parse_rule_and_or(&ast_child, lexer);
        if (status != PARSER_OK)
            break;
        ast_cmd_list_add_ast(ast_list, ast_child);
    }

    if (status == PARSER_UNEXPECTED_EOF)
        goto error;
    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return status;
}
