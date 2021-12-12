#include "rule_pipeline.h"

#include <lexer/lexer.h>
#include <parser/negation/ast_negation.h>
#include <parser/simple_cmd/ast_simple_cmd.h>
#include <stdlib.h>
#include <vector/vector.h>

#include "ast_pipeline.h"

/**
 * @brief expand_pipeline: '|' ('\n')* command
 * @return
 */
static enum parser_status parse_rule_e_pipeline(struct ast_node **ast,
                                                struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);

    enum parser_status status;

    if (!is_op(lexer_peek(*lexer), OP_PIPE))
    {
        status = PARSER_UNEXPECTED_TOKEN;
        goto error;
    }
    token_free(lexer_pop(*lexer));

    while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
    {
        struct token *tok = lexer_pop(*lexer);
        token_free(tok);
    }

    status = parse_rule_cmd(ast, lexer); // command
    if (status != PARSER_OK)
        goto error;

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    return status;
}

/**
 * @brief pipeline: ['!'] command (expand_pipeline)*
 * @return
 */
enum parser_status parse_rule_pipeline(struct ast_node **ast,
                                       struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);
    bool is_negation = false;

    enum parser_status status;

    if (is_rw(lexer_peek(*lexer), RW_NEG))
    {
        is_negation = true;
        token_free(lexer_pop(*lexer));
        // create an ast_negation node
        struct ast_negation *ast_negation = ast_negation_init();
        // attach ast_negation to ast
        *ast = (struct ast_node *)ast_negation;
    }

    // Create an empy AST pipeline
    struct ast_pipeline *ast_pipeline = ast_pipeline_init();
    if (is_negation == true)
    {
        // attach ast_pipeline to ast_negation
        struct ast_negation *ast_negation = (struct ast_negation *)*ast;
        ast_negation->child = (struct ast_node *)ast_pipeline;
    }
    else
    {
        // attach ast_pipeline to ast
        *ast = (struct ast_node *)ast_pipeline;
    }

    struct ast_node *ast_child = NULL;
    status = parse_rule_cmd(&ast_child, lexer); // command
    ast_pipeline_add_ast(ast_pipeline, ast_child);
    if (status != PARSER_OK)
        goto error;

    ast_child = NULL;
    while ((status = parse_rule_e_pipeline(&ast_child, lexer)) == PARSER_OK)
    {
        ast_pipeline_add_ast(ast_pipeline, ast_child);
    }

    lexer_free_without_pretokens(saved_lexer);
    if (status == PARSER_UNEXPECTED_EOF)
        return PARSER_UNEXPECTED_EOF;
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return status;
}
