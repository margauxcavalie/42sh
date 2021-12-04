#include "rule_pipeline.h"

#include <lexer/lexer.h>
#include <parser/simple_cmd/ast_simple_cmd.h>
#include <parser/negation/ast_negation.h>
#include <stdlib.h>
#include <vector/vector.h>

#include "ast_pipeline.h"

/**
 * @brief pipeline: ['!'] command ( '|' ('\n')* command )*
 * @return
 */
enum parser_status parse_rule_pipeline(struct ast_node **ast,
                                       struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);
    bool is_negation = false;

    if (is_op(lexer_peek(*lexer), OP_NEG))
    {
        is_negation = true;
        token_free(lexer_pop(*lexer));
        // create an ast_negation node
        struct ast_negation *ast_negation = ast_negation_init();
        // attach ast_negation to ast
        *ast = (struct ast_node *)ast_negation;
    }

    struct ast_pipeline *ast_pipeline =
        ast_pipeline_init(); // Create an empy AST pipeline
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
    enum parser_status status = parse_rule_cmd(&ast_child, lexer); // command
    ast_pipeline_add_ast(ast_pipeline, ast_child);
    if (status != PARSER_OK)
        goto error;

    while (is_op(lexer_peek(*lexer), OP_PIPE)) // ( '|' ('\n')* command )*
    {
        struct token *tok = lexer_pop(*lexer);
        token_free(tok);

        while (is_op(lexer_peek(*lexer), OP_LINEFEED)) // ('\n')*
        {
            struct token *tok = lexer_pop(*lexer);
            token_free(tok);
        }

        struct ast_node *ast_child = NULL;
        enum parser_status status =
            parse_rule_cmd(&ast_child, lexer); // command
        if (status != PARSER_OK)
            break;
        ast_pipeline_add_ast(ast_pipeline, ast_child);
    }

    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;
error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}