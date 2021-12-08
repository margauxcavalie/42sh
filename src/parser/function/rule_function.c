#include "rule_function.h"

#include <lexer/lexer.h>
#include <lexer/token.h>
#include <parser/parser.h>
#include <string.h>

#include "ast_function.h"


/**
 * @brief funcdec: WORD '(' ')' ('\n')* shell_command
 * 
 * @param ast 
 * @param lexer 
 * @return enum parser_status 
 */
enum parser_status parse_rule_funcdec(struct ast_node **ast,
                                     struct lexer **lexer)
{
    struct lexer *saved_lexer = save_lexer(*lexer);
    enum parser_status status = PARSER_OK;

    if (!is_word(lexer_peek(*lexer)))
    {
        status = PARSER_UNEXPECTED_TOKEN;
        goto error;
    }
    struct token *word_tok = lexer_pop(*lexer);
    char function_name[1024];
    strcpy(function_name, word_tok->data.word);
    token_free(word_tok);

    if (!is_op(lexer_peek(*lexer), OP_LPARENTHESE))
    {
        status = PARSER_UNEXPECTED_TOKEN;
        goto error;
    }
    token_free(lexer_pop(*lexer));

    if (!is_op(lexer_peek(*lexer), OP_RPARENTHESE))
    {
        status = PARSER_UNEXPECTED_TOKEN;
        goto error;
    }
    token_free(lexer_pop(*lexer));

    while(is_op(lexer_peek(*lexer), OP_LINEFEED))
    {
        token_free(lexer_pop(*lexer));
    }

    struct ast_function *ast_function = ast_function_init(function_name);
    *ast = (struct ast_node *)ast_function;

    status = parse_rule_shell_cmd(&(ast_function->child), lexer);
    if (status != PARSER_OK)
        goto error;
    
    lexer_free_without_pretokens(saved_lexer);
    return PARSER_OK;

error:
    restore_lexer(lexer, saved_lexer);
    ast_node_free_detach(ast);
    return status;
}
