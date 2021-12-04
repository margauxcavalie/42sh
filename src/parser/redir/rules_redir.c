#include "rules_redir.h"

#include <lexer/lexer.h>
#include <stdlib.h>

#include "ast_redir.h"

/**
 * @brief (temporary version)
 * redirection: [IONUMBER] op_redir WORD
 * @return
 */
enum parser_status parse_rule_redirection(struct ast_node **ast,
                                          struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    struct ast_redir *ast_redir = ast_redir_init();

    bool custom_fd = false; // true if the user enter a custom ionumber
    if (is_ionumber(lexer_peek(*lexer)))
    {
        custom_fd = true;
        struct token *tok = lexer_pop(*lexer);
        ast_redir_set_fd(ast_redir, tok->data.io_number);
        token_free(tok);
    }

    if (!is_op(lexer_peek(*lexer), OP_REDIR))
        goto error;
    struct token *tok_op = lexer_pop(*lexer);
    ast_redir_set_type(ast_redir, tok_op->data.op_data.data.redir_data.type);
    if (custom_fd == false)
        ast_redir_set_fd(ast_redir,
                         tok_op->data.op_data.data.redir_data.default_fd);
    token_free(tok_op);

    if (!is_word(lexer_peek(*lexer)))
        goto error;
    struct token *tok_word = lexer_pop(*lexer);
    ast_redir_set_file(ast_redir, tok_word->data.word);
    token_free(tok_word);

    // add the ast parent into the redirect child and attach
    ast_redir_set_child(ast_redir, *ast);
    *ast = (struct ast_node *)ast_redir;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free((struct ast_node *)ast_redir);
    return PARSER_UNEXPECTED_TOKEN;
}
