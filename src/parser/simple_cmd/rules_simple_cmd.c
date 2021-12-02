#include "rules_simple_cmd.h"

#include <lexer/lexer.h>
#include <stdlib.h>

#include "ast_simple_cmd.h"

/**
 * @brief attach a new ast_simple_cmd node at the adress of the node ast
 *
 * @param ast
 * @return struct ast_simple_cmd *
 */
static struct ast_simple_cmd *ast_node_simple_cmd_attach(struct ast_node **ast)
{
    struct ast_simple_cmd *ast_simple_cmd = ast_simple_cmd_init();
    *ast = (struct ast_node *)ast_simple_cmd;
    return ast_simple_cmd;
}

/**
 * @brief (temporary version)
 * simple_command: WORD+.
 * Error if there are no words
 * @return
 */
enum parser_status parse_rule_simple_cmd(struct ast_node **ast,
                                         struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    // create a new AST node, and attach it to the ast pointer
    struct ast_simple_cmd *ast_simple = ast_node_simple_cmd_attach(ast);

    // ERROR
    if (lexer_peek(*lexer)->type != TOKEN_WORD)
        goto error;

    while (lexer_peek(*lexer)->type == TOKEN_WORD) // WORD+
    {
        struct token *tok = lexer_pop(*lexer);
        // adds the node to the AST's vector
        ast_simple = ast_simple_cmd_add_param(ast_simple, tok->data.word);
        token_free(tok);
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;

error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}
