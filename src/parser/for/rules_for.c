#include "rules_for.h"

#include <lexer/lexer.h>
#include <parser/while_until/rules_while_until.h>
#include <stdio.h> // DELETE
#include <stdlib.h>

#include "ast_for.h"

/**
 * @brief
 * rule: ([';']|[('\n')* 'in' (WORD)* (';'|'\n')])
 * @return
 */
static enum parser_status parse_rule_for_annex(struct ast_for **ast_for,
                                               struct lexer **lexer)
{
    if (is_op(lexer_peek(*lexer), OP_SEMICOLON) == true) // ';'
    {
        token_free(lexer_pop(*lexer));
        return PARSER_OK;
    }

    struct lexer *saved_lexer = save_lexer(*lexer);
    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
    {
        token_free(lexer_pop(*lexer));
    }

    if (is_rw(lexer_peek(*lexer), RW_IN) == false) // 'in'
    {
        restore_lexer(lexer, saved_lexer);
        return PARSER_OK;
    }
    token_free(lexer_pop(*lexer));
    
    while (is_word(lexer_peek(*lexer)) == true) // WORD*
    {
        // DO SOMETHING TO GET THE WORDS
        ast_for_add_word(*ast_for, (lexer_peek(*lexer)));
        token_free(lexer_pop(*lexer));
    }

    if (is_op(lexer_peek(*lexer), OP_SEMICOLON) == false) // ';'
        if (is_op(lexer_peek(*lexer), OP_LINEFEED) == false) // '\n'
        {
            restore_lexer(lexer, saved_lexer);
            return PARSER_OK;
        }

    token_free(lexer_pop(*lexer));
    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
}

/**
 * @brief
 * rule_for: for WORD ([';']|[('\n')* 'in' (WORD)* (';'|'\n')]) ('\n')* do_group
 * @return
 */
enum parser_status parse_rule_for(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    struct ast_for *ast_for = ast_for_init();
    *ast = (struct ast_node *)ast_for; // attach AST_for to AST

    struct ast_node *ast_for_body = NULL;

    if (is_rw(lexer_peek(*lexer), RW_FOR) == false) // 'for'
        goto error;
    token_free(lexer_pop(*lexer));

    if (is_word(lexer_peek(*lexer)) == false) // WORD
        goto error;
    ast_for_add_word(ast_for, (lexer_peek(*lexer)));
    token_free(lexer_pop(*lexer));

    if (is_op(lexer_peek(*lexer), OP_SEMICOLON) == true) // ';'
        token_free(lexer_pop(*lexer));

    // ([';']|[('\n')* 'in' (WORD)* (';'|'\n')])
    parse_rule_for_annex(&ast_for, lexer);

    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
    {
        token_free(lexer_pop(*lexer));
    }

    enum parser_status status =
        parse_rule_do_group(&ast_for_body, lexer); // do_group
    // Set the body
    ast_for_set_body(ast_for, ast_for_body);
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;

error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}
