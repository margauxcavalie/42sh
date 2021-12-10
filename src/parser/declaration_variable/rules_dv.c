#include "rules_dv.h"

#include <lexer/lexer.h>
#include <stdlib.h>
#include <var_expansion/var_expansion.h>

#include "ast_dv.h"
/**
 * @brief
 * declaration_variable : ASSIGNEMENT_WORD
 * @param ast
 * @param lexer
 * @return enum parser_status
 */
enum parser_status parse_rule_dv(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    struct ast_dv *ast_dv = ast_dv_init();

    struct token *tok_word = lexer_peek(*lexer);
    if (!is_assignement_word(tok_word))
    {
        goto error;
    }
    ast_dv_set_key_value_pair(ast_dv, tok_word->data.word);
    token_free(lexer_pop(*lexer));
    // add the ast parent into the dv child and attach
    ast_dv_set_child(ast_dv, *ast);
    *ast = (struct ast_node *)ast_dv;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free((struct ast_node *)ast_dv);
    return PARSER_UNEXPECTED_TOKEN;
}
