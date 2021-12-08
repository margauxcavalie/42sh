#include "rules_if.h"

#include <lexer/lexer.h>
#include <parser/cmd_list/rules_cmd_list.h>
#include <stdlib.h>

#include "ast_if.h"

/**
 * @brief (temporary version)
 * else_clause: ELSE compound_list
 *            | ELIF compound_list THEN compound_list [else_clause]
 * @return
 */
static enum parser_status parse_rule_else_clause(struct ast_node **ast,
                                                 struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    if (is_rw(lexer_peek(*lexer), RW_ELSE) == true)
    {
        token_free(lexer_pop(*lexer));
        status = parse_rule_compound_list(ast, lexer);
        if (status != PARSER_OK)
            goto error;

        lexer_free_without_pretokens(saved_lexer);
        return PARSER_OK;
    }

    if (is_rw(lexer_peek(*lexer), RW_ELIF) == true)
    {
        struct ast_if *ast_if = ast_if_init();
        *ast = (struct ast_node *)ast_if; // attach ast_if to ast

        token_free(lexer_pop(*lexer));
        struct ast_node *ast_elif_condition = NULL;
        struct ast_node *ast_elif_body = NULL;
        struct ast_node *ast_elif_else = NULL;

        status = parse_rule_compound_list(&ast_elif_condition, lexer);
        // Set condition_body
        ast_if_set_condition(ast_if, ast_elif_condition);
        if (status != PARSER_OK)
            goto error_elif;

        if (is_rw(lexer_peek(*lexer), RW_THEN) == false)
        {
            status = get_error_status(lexer_peek(*lexer));
            goto error_elif;
        }
        token_free(lexer_pop(*lexer));

        status = parse_rule_compound_list(&ast_elif_body, lexer);
        // Set if_body
        ast_if_set_body(ast_if, ast_elif_body);
        if (status != PARSER_OK)
            goto error_elif;

        parse_rule_else_clause(&ast_elif_else, lexer);
        // Set else_body
        ast_if_set_else(ast_if, ast_elif_else);

        lexer_free_without_pretokens(saved_lexer);
        return PARSER_OK;
    error_elif:
        ast_node_free_detach(ast);
        goto error;
    }
    status = PARSER_UNEXPECTED_TOKEN;
error:
    restore_lexer(lexer, saved_lexer);
    return status;
}

/**
 * @brief (temporary version)
 * rule_if: IF compound_list THEN compound_list [else_clause] FI
 * @return
 */
enum parser_status parse_rule_if(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    struct ast_if *ast_if = ast_if_init();
    *ast = (struct ast_node *)ast_if; // attach ast_if to ast

    struct ast_node *ast_if_condition = NULL;
    struct ast_node *ast_if_body = NULL;
    struct ast_node *ast_else_body = NULL;

    if (is_rw(lexer_peek(*lexer), RW_IF) == false)
    {
        status = get_error_status(lexer_peek(*lexer));
        goto error;
    }
    token_free(lexer_pop(*lexer));

    status = parse_rule_compound_list(&ast_if_condition, lexer);
    ast_if_set_condition(ast_if, ast_if_condition); // Set the condition_body
    if (status != PARSER_OK)
        goto error;

    if (is_rw(lexer_peek(*lexer), RW_THEN) == false)
    {
        status = get_error_status(lexer_peek(*lexer));
        goto error;
    }
    token_free(lexer_pop(*lexer));

    status = parse_rule_compound_list(&ast_if_body, lexer);
    ast_if_set_body(ast_if, ast_if_body); // Set the if_body
    if (status != PARSER_OK)
        goto error;

    status = parse_rule_else_clause(&ast_else_body, lexer);
    ast_if_set_else(ast_if, ast_else_body); // Set the else_body
    if (status == PARSER_UNEXPECTED_EOF)
        goto error;

    if (is_rw(lexer_peek(*lexer), RW_FI) == false)
    {
        status = get_error_status(lexer_peek(*lexer));
        goto error;
    }
    token_free(lexer_pop(*lexer));

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return status;
}
