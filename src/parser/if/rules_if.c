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
                                                 struct lexer *lexer)
{
    if (is_rw(lexer_peek(lexer), RW_ELSE) == true)
    {
        token_free(lexer_pop(lexer));
        enum parser_status status = parse_rule_compound_list(ast, lexer);
        if (status != PARSER_OK)
            return PARSER_UNEXPECTED_TOKEN;
        return PARSER_OK;
    }

    if (is_rw(lexer_peek(lexer), RW_ELIF) == true)
    {
        struct ast_if *ast_if = ast_if_init();
        *ast = (struct ast_node *)ast_if; // attach ast_if to ast

        token_free(lexer_pop(lexer));
        struct ast_node *ast_elif_condition = NULL;
        struct ast_node *ast_elif_body = NULL;
        struct ast_node *ast_elif_else = NULL;

        enum parser_status status =
            parse_rule_compound_list(&ast_elif_condition, lexer);
        ast_if_set_condition(ast_if,
                             ast_elif_condition); // Set condition_body

        if (status != PARSER_OK)
            goto error_elif;

        if (is_rw(lexer_peek(lexer), RW_THEN) == false)
            goto error_elif;
        token_free(lexer_pop(lexer));

        status = parse_rule_compound_list(&ast_elif_body, lexer);
        ast_if_set_body(ast_if,
                        ast_elif_body); // Set if_body

        if (status != PARSER_OK)
            goto error_elif;

        parse_rule_else_clause(&ast_elif_else, lexer);
        ast_if_set_else(ast_if,
                        ast_elif_else); // Set else_body

        return PARSER_OK;
    error_elif:
        ast_node_free_detach(ast);
        return PARSER_UNEXPECTED_TOKEN;
    }

    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief (temporary version)
 * rule_if: IF compound_list THEN compound_list [else_clause] FI
 * @return
 */
enum parser_status parse_rule_if(struct ast_node **ast, struct lexer *lexer)
{
    struct ast_if *ast_if = ast_if_init();
    *ast = (struct ast_node *)ast_if; // attach ast_if to ast

    struct ast_node *ast_if_condition = NULL;
    struct ast_node *ast_if_body = NULL;
    struct ast_node *ast_else_body = NULL;

    if (is_rw(lexer_peek(lexer), RW_IF) == false)
        goto error;
    token_free(lexer_pop(lexer));

    enum parser_status status =
        parse_rule_compound_list(&ast_if_condition, lexer);
    ast_if_set_condition(ast_if, ast_if_condition); // Set the condition_body

    if (status != PARSER_OK)
        goto error;

    if (is_rw(lexer_peek(lexer), RW_THEN) == false)
        goto error;
    token_free(lexer_pop(lexer));

    status = parse_rule_compound_list(&ast_if_body, lexer);
    ast_if_set_body(ast_if, ast_if_body); // Set the if_body

    if (status != PARSER_OK)
        goto error;

    parse_rule_else_clause(&ast_else_body, lexer);
    ast_if_set_else(ast_if, ast_else_body); // Set the else_body

    if (is_rw(lexer_peek(lexer), RW_FI) == false)
        goto error;
    token_free(lexer_pop(lexer));

    return PARSER_OK;
error:
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}
