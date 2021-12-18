#include "rules_case.h"

#include <lexer/lexer.h>
#include <parser/cmd_list/rules_cmd_list.h>
#include <stdlib.h>
#include <string.h>
#include <vector/vector.h>

#include "ast_case.h"

/**
 * @brief
 * under_rule2: ('|' WORD)
 * @return
 */
static enum parser_status
parse_rule_case_under_rule2(struct lexer **lexer, struct vector **tmp_patterns)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    if (is_op(lexer_peek(*lexer), OP_PIPE)) // '|'
    {
        token_free(lexer_pop(*lexer));
        if (is_word(lexer_peek(*lexer)) == false)
            goto error;

        vector_append(*tmp_patterns, strdup(lexer_peek(*lexer)->data.word));
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
 * case_item: ['('] WORD under_rule2* ')' ('\n')* [ compound_list ]
 * @return
 */
static enum parser_status parse_rule_case_item(struct ast_case **ast_case,
                                               struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);
    enum parser_status status;

    struct vector *tmp_patterns = vector_init(5);
    struct ast_node *tmp_body = NULL;

    if (is_op(lexer_peek(*lexer), OP_LPARENTHESE)) // '('
        token_free(lexer_pop(*lexer));

    if (is_word(lexer_peek(*lexer)) == false) // WORD
        goto error;
    vector_append(tmp_patterns, strdup(lexer_peek(*lexer)->data.word));
    token_free(lexer_pop(*lexer));

    // under_rule2*
    while ((status = parse_rule_case_under_rule2(lexer, &tmp_patterns))
           == PARSER_OK)
    {
        ;
    }

    if (is_op(lexer_peek(*lexer), OP_RPARENTHESE) == false) // ')'
        goto error;
    token_free(lexer_pop(*lexer));

    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
    {
        token_free(lexer_pop(*lexer));
    }

    parse_rule_compound_list(&tmp_body, lexer); // [ compound_list ]
    ast_case_add_body(*ast_case, tmp_body);
    ast_case_add_pattern(*ast_case, tmp_patterns);

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;

error:
    restore_lexer(lexer, saved_lexer);
    vector_destroy(tmp_patterns);
    return status;
}

/**
 * @brief
 * under_rule: ';;' ('\n')* case_item
 * @return
 */
static enum parser_status parse_rule_case_under_rule(struct ast_case **ast_case,
                                                     struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);
    enum parser_status status;

    if (is_op(lexer_peek(*lexer), OP_DOUBLE_SEMICOLON)) // ';;'
    {
        token_free(lexer_pop(*lexer));
        while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
        {
            token_free(lexer_pop(*lexer));
        }

        status = parse_rule_case_item(ast_case, lexer); // case_item
        if (status != PARSER_OK)
            goto error;

        lexer_free_without_pretokens(saved_lexer);
        return PARSER_OK;
    }

    status = PARSER_UNEXPECTED_TOKEN;

error:
    restore_lexer(lexer, saved_lexer);
    return status;
}

/**
 * @brief
 * case_clause: case_item under_rule* [;;] ('\n')*
 * @return
 */
static enum parser_status parse_rule_case_clause(struct ast_case **ast_case,
                                                 struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);
    enum parser_status status;

    status = parse_rule_case_item(ast_case, lexer); // case_item
    if (status != PARSER_OK)
        goto error;

    // (';;' ('\n')* case_item)*
    while ((status = parse_rule_case_under_rule(ast_case, lexer)) == PARSER_OK)
    {
        ;
    }

    if (is_op(lexer_peek(*lexer), OP_DOUBLE_SEMICOLON)) // [;;]
        token_free(lexer_pop(*lexer));

    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
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
 * @brief
 * rule_case: Case WORD ('\n')* 'in' ('\n')* [case_clause] Esac
 *
 * 'esac'
 * @return
 */
enum parser_status parse_rule_case(struct ast_node **ast, struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    struct ast_case *ast_case = ast_case_init();
    *ast = (struct ast_node *)ast_case; // attach ast_case to ast

    if (is_rw(lexer_peek(*lexer), RW_CASE) == false) // 'case'
    {
        status = get_error_status(lexer_peek(*lexer));
        goto error;
    }
    token_free(lexer_pop(*lexer));

    if (is_word(lexer_peek(*lexer)) == false) // WORD
    {
        status = PARSER_UNEXPECTED_TOKEN;
        goto error;
    }
    ast_case_set_reference(ast_case, strdup(lexer_peek(*lexer)->data.word));
    token_free(lexer_pop(*lexer));

    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
    {
        token_free(lexer_pop(*lexer));
    }

    if (is_rw(lexer_peek(*lexer), RW_IN) == false) // 'in'
    {
        status = PARSER_UNEXPECTED_TOKEN;
        goto error;
    }
    token_free(lexer_pop(*lexer));

    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
    {
        token_free(lexer_pop(*lexer));
    }

    status = parse_rule_case_clause(&ast_case, lexer); // case_clause

    if (is_rw(lexer_peek(*lexer), RW_ESAC) == false) // 'esac'
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
