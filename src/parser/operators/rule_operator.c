#include "rule_operator.h"

#include <lexer/lexer.h>
#include <parser/pipeline/rule_pipeline.h>

#include "ast_operator.h"

/**
 * @brief and_or: pipeline (('&&'|'||') ('\n')* pipeline)*
 *
 * @param ast
 * @param lexer
 * @return enum parser_status
 */
enum parser_status parse_rule_operator(struct ast_node **ast,
                                       struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status = parse_rule_pipeline(ast, lexer);
    if (status != PARSER_OK)
        goto error;

    while ((is_op(lexer_peek(*lexer), OP_AND) || (is_op(lexer_peek(*lexer), OP_OR))))
    {
        struct ast_operator *ast_operator = NULL;
        if (is_op(lexer_peek(*lexer), OP_AND))
        {
            ast_operator = ast_operator_init(OP_AND);
            ast_operator->left = *ast;
            *ast = (struct ast_node *)ast_operator;
        }
        else if (is_op(lexer_peek(*lexer), OP_OR))
        {
            ast_operator = ast_operator_init(OP_OR);
            ast_operator->left = *ast;
            *ast = (struct ast_node *)ast_operator;
        }

        struct token *tok = lexer_pop(*lexer);
        token_free(tok);

         while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
        {
            struct token *tok = lexer_pop(*lexer);
            token_free(tok);
        }

        enum parser_status status = parse_rule_pipeline(&ast_operator->right, lexer);
        if (status != PARSER_OK)
            goto error;
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;

error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}