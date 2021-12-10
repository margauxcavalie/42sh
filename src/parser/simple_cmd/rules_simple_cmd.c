#include "rules_simple_cmd.h"

#include <lexer/lexer.h>
#include <parser/declaration_variable/ast_dv.h>
#include <parser/declaration_variable/rules_dv.h>
#include <parser/redir/ast_redir.h>
#include <parser/redir/rules_redir.h>
#include <stdlib.h>
#include <string.h>

#include "ast_simple_cmd.h"

static void set_child(struct ast_node *parent, struct ast_node *child)
{
    if (parent->type == AST_REDIR)
    {
        struct ast_redir *ast = (struct ast_redir *)parent;
        ast_redir_set_child(ast, child);
    }
    else if (parent->type == AST_DV)
    {
        struct ast_dv *ast = (struct ast_dv *)parent;
        ast_dv_set_child(ast, child);
    }
    else
        errx(1, "parser: prefix behaviour unknown");
}

/**
 * @brief (temporary version)
 * prefix: declaration_variable
 *       | redirection
 * @return
 */
static enum parser_status parse_rule_prefix(struct ast_node **ast,
                                            struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status = parse_rule_dv(ast, lexer);
    if (status == PARSER_OK)
    {
        lexer_free_without_pretokens(saved_lexer);
        return PARSER_OK;
    }

    status = parse_rule_redirection(ast, lexer);
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return status;
}

/**
 * @brief (temporary version)
 * element: WORD
 *        | redirection
 * @return
 */
static enum parser_status parse_rule_element(struct ast_node **ast, char **word,
                                             struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    if (is_word(lexer_peek(*lexer)))
    {
        struct token *tok = lexer_pop(*lexer);
        *word = strdup(tok->data.word);
        token_free(tok);

        lexer_free_without_pretokens(saved_lexer);
        return PARSER_OK;
    }
    status = parse_rule_redirection(ast, lexer);
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return status;
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

    enum parser_status status;

    // create a new AST node, and attach it to the ast pointer
    struct ast_simple_cmd *ast_simple_cmd = ast_simple_cmd_init();
    *ast = (struct ast_node *)ast_simple_cmd;

    struct ast_node *ast_cur = (struct ast_node *)ast_simple_cmd;
    ast = &ast_cur;

    struct ast_node *last_parent_ast = NULL;

    size_t prefix_count = 0;
    while (true)
    {
        status = parse_rule_prefix(ast, lexer);
        if (status != PARSER_OK)
            break;
        if (last_parent_ast == NULL)
            *saved_ast = *ast;
        else
            set_child(last_parent_ast, *ast);
        last_parent_ast = *ast;
        *ast = (struct ast_node *)ast_simple_cmd;
        prefix_count += 1;
    }

    size_t elements_count = 0;
    while (true)
    {
        char *word = NULL;
        // if return a word, we need to free it
        status = parse_rule_element(ast, &word, lexer);
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
        else // if there is a redirection
        {
            if (last_parent_ast == NULL)
                *saved_ast = *ast;
            else
                set_child(last_parent_ast, *ast);
            last_parent_ast = *ast;
            *ast = (struct ast_node *)ast_simple_cmd;
            // prefix_count += 1;
        }
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;

error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(saved_ast);
    return status;
}
