#include <parser/ast_simple_cmd_node.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
static void ast_simple_cmd_free(struct ast_node *ast)
{
    struct ast_simple_cmd_node *ast_simple_cmd =
        (struct ast_simple_cmd_node *)ast;
    vector_apply_on_elts(ast_simple_cmd->params, &free);
    vector_destroy(ast_simple_cmd->params);
}

/**
 * @brief Prints the content of an AST simple_cmd. Isn't recursive.
 */
static void ast_simple_cmd_print(struct ast_node *ast)
{
    struct ast_simple_cmd_node *ast_simple_cmd =
        (struct ast_simple_cmd_node *)ast;
    struct vector *v = ast_simple_cmd->params;
    if (!v || v->size == 0) // Vector is empty or non-existent
    {
        printf("\nERROR: command without words\n"); // usually impossible since
                                                    // a command must have at
                                                    // least one word
        return;
    }

    for (size_t i = 0; i < v->size - 1; i++)
    {
        char *str = v->data[i];
        printf("%s ", str);
    }

    char *str = v->data[v->size - 1];
    printf("%s", str);
}

/**
 * @brief Initializes a simple_command AST. Its vector has a size 5
 */
static struct ast_simple_cmd_node *ast_simple_cmd_init(void)
{
    struct ast_simple_cmd_node *new_ast = xmalloc(
        sizeof(struct ast_simple_cmd_node)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_SIMPLE_CMD;
    base->node_free = &ast_simple_cmd_free;
    base->node_print = &ast_simple_cmd_print;
    // base->node_exec = TODO

    new_ast->params = vector_init(5);
    return new_ast;
}

/**
 * @brief Adds a new parameter to the ast. Mallocs a copy of the new parameter,
 * but does not free the old one.
 * @param param needs to have a '\\0' at the end
 */
static struct ast_simple_cmd_node *
ast_simple_cmd_add_param(struct ast_simple_cmd_node *ast, char *param)
{
    size_t len = strlen(param);
    // new allocation to avoid double free
    char *tmp = zalloc(sizeof(char) * (len + 1));
    strncpy(tmp, param, len);
    ast->params = vector_append(ast->params, tmp);
    return ast;
}

/**
 * @brief attach a new ast_simple_cmd node at the adress of the node ast
 *
 * @param ast
 * @return struct ast_simple_cmd_node *
 */
static struct ast_simple_cmd_node *
ast_node_simple_cmd_attach(struct ast_node **ast)
{
    struct ast_simple_cmd_node *ast_simple_cmd = ast_simple_cmd_init();
    *ast = (struct ast_node *)ast_simple_cmd;
    return ast_simple_cmd;
}

/**
 * @brief (temporary version)
 * simple_command: WORD+.
 * Error if there are no words
 * @return
 */
enum parser_status parse_rule_simple_command(struct ast_node **ast,
                                             struct lexer *lexer)
{
    // ERROR
    if (lexer_peek(lexer)->type != TOKEN_WORD)
        return PARSER_UNEXPECTED_TOKEN;

    // create a new AST node, and attach it to the ast pointer
    struct ast_simple_cmd_node *ast_simple = ast_node_simple_cmd_attach(ast);

    while (lexer_peek(lexer)->type == TOKEN_WORD) // WORD+
    {
        struct token *tok = lexer_pop(lexer);
        // adds the node to the AST's vector
        ast_simple = ast_simple_cmd_add_param(ast_simple, tok->data.word);
        token_free(tok);
    }

    return PARSER_OK;
}