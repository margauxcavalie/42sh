#include "rules_simple_cmd.h"

#include <lexer/lexer.h>
#include <parser/redir/rules_redir.h>
#include <stdlib.h>
#include <string.h>

#include "ast_simple_cmd.h"

/**
 * @brief (temporary version)
 * prefix: redirection
 * @return
 */
enum parser_status parse_rule_prefix(struct ast_node **ast,
                                     struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status = parse_rule_redirection(ast, lexer);
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief (temporary version)
 * element: WORD
 *        | redirection
 * @return
 */
enum parser_status parse_rule_element(struct ast_node **ast, char **word,
                                      struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    if (is_word(lexer_peek(*lexer)))
    {
        struct token *tok = lexer_pop(*lexer);
        *word = strdup(tok->data.word);
        token_free(tok);

        lexer_free_without_pretokens(saved_lexer);
        return PARSER_OK;
    }
    enum parser_status status = parse_rule_redirection(ast, lexer);
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief (temporary version)
 * simple_command: (prefix)+
 *              |  (prefix)* (element)+
 * @return
 */
enum parser_status parse_rule_simple_cmd(struct ast_node **ast,
                                         struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    struct ast_node **saved_ast = ast;

    // create a new AST node, and attach it to the ast pointer
    struct ast_simple_cmd *ast_simple_cmd = ast_simple_cmd_init();
    *ast = (struct ast_node *)ast_simple_cmd;

    size_t prefix_count = 0;
    while (true)
    {
        enum parser_status status = parse_rule_prefix(ast, lexer);
        if (status != PARSER_OK)
            break;
        prefix_count += 1;
    }

    size_t elements_count = 0;
    while (true)
    {
        char *word = NULL;
        // if return a word, we need to free it
        enum parser_status status = parse_rule_element(ast, &word, lexer);
        if (status != PARSER_OK)
        {
            if (prefix_count == 0 && elements_count == 0)
                goto error;
            break;
        }
        elements_count += 1;
        if (word != NULL) // if there is a cmd word
        {
            ast_simple_cmd = ast_simple_cmd_add_param(ast_simple_cmd, word);
            free(word);
        }
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;

error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(saved_ast);
    return PARSER_UNEXPECTED_TOKEN;
}
