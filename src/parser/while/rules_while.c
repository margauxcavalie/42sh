#include "rules_while.h"

#include <lexer/lexer.h>
#include <parser/cmd_list/rules_cmd_list.h>
#include <stdlib.h>

#include "ast_while.h"

/**
 * @brief (temporary version)
 * do_group: DO compound_list DONE
 * @return
 */
enum parser_status parse_rule_do_group(struct ast_node **ast,
                                       struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    if (is_rw(lexer_peek(*lexer), RW_DO) == true)
    {
        token_free(lexer_pop(*lexer));
        enum parser_status status = parse_rule_compound_list(ast, lexer);
        if (status != PARSER_OK)
            goto error;

        if (is_rw(lexer_peek(*lexer), RW_DONE) == false)
            goto error;
        token_free(lexer_pop(*lexer));

        lexer_free_without_pretokens(saved_lexer);
        return PARSER_OK;
    }

error:
    restore_lexer(lexer, saved_lexer);
    return PARSER_UNEXPECTED_TOKEN;
}
/**
 * @brief
 * rule_while: WHILE compound_list do_group
 * @return
 */
enum parser_status parse_rule_while(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    struct ast_while_until *ast_while = ast_while_init();
    *ast = (struct ast_node *)ast_while; // attach AST_while to AST

    struct ast_node *ast_while_condition = NULL;
    struct ast_node *ast_while_body = NULL;

    if (is_rw(lexer_peek(*lexer), RW_WHILE) == false)
        goto error;
    token_free(lexer_pop(*lexer));

    enum parser_status status =
        parse_rule_compound_list(&ast_while_condition, lexer);
    // Set the condition
    ast_while_until_set_condition(ast_while, ast_while_condition);
    if (status != PARSER_OK)
        goto error;

    status = parse_rule_do_group(&ast_while_body, lexer);
    // Set the body
    ast_while_until_set_body(ast_while, ast_while_body);
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief
 * rule_until: UNTIL compound_list do_group
 * @return
 */
enum parser_status parse_rule_until(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    struct ast_while_until *ast_until = ast_until_init();
    *ast = (struct ast_node *)ast_until; // attach AST_while to AST

    struct ast_node *ast_until_condition = NULL;
    struct ast_node *ast_until_body = NULL;

    if (is_rw(lexer_peek(*lexer), RW_UNTIL) == false)
        goto error;
    token_free(lexer_pop(*lexer));

    enum parser_status status =
        parse_rule_compound_list(&ast_until_condition, lexer);
    // Set the condition
    ast_while_until_set_condition(ast_until, ast_until_condition);
    if (status != PARSER_OK)
        goto error;

    status = parse_rule_do_group(&ast_until_body, lexer);
    // Set the body
    ast_while_until_set_body(ast_until, ast_until_body);
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}
