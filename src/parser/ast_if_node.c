#include <parser/ast_cmd_list_node.h>
#include <parser/ast_simple_cmd_node.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
static void ast_if_free(struct ast_node *ast)
{
    struct ast_if_node *ast_if = (struct ast_if_node *)ast;
    ast_node_free(ast_if->condition);
    ast_node_free(ast_if->if_body);
    ast_node_free(ast_if->else_body);
}

/**
 * @brief Prints the content of an AST if.
 */
static void ast_if_print(struct ast_node *ast)
{
    struct ast_if_node *ast_if = (struct ast_if_node *)ast;
    printf("if { ");
    ast_node_print(ast_if->condition);
    printf(" }; ");
    printf("then { ");
    ast_node_print(ast_if->if_body);
    printf(" }; ");
    if (ast_if->else_body != NULL)
    {
        printf("else { ");
        ast_node_print(ast_if->else_body);
        printf(" }; ");
    }
}

static int ast_if_exec(struct ast_node *ast)
{
    struct ast_if_node *ast_if = (struct ast_if_node *)ast;
    if (!ast_node_exec(ast_if->condition)) // if it returns 0
    {
        return ast_node_exec(ast_if->if_body);
    }
    if (ast_if->else_body != NULL)
    {
        return ast_node_exec(ast_if->else_body);
    }
    return 0;
}

/**
 * @brief Initializes a cmd_list AST. Its vector has a size 5
 */
static struct ast_if_node *ast_if_init(struct ast_node *condition, struct ast_node *if_body)
{
    struct ast_cmd_list_node *new_ast = xmalloc(
        sizeof(struct ast_cmd_list_node)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_IF;
    base->node_free = &ast_if_free;
    base->node_print = &ast_if_print;
    base->node_exec = &ast_if_exec;

    new_ast->condition = condition;
    new_ast->if_body = if_body;
    return new_ast;
}

/**
 * @brief Adds a new AST to the already existing AST.
 * @param ast_elt Newly added AST
 */
static void ast_cmd_list_add_ast(struct ast_cmd_list_node *ast,
                                 struct ast_node *ast_elt)
{
    ast->ast_list = vector_append(ast->ast_list, ast_elt);
}

/**
 * @brief (temporary version)
 * list: command (';' command)* [';']
 * @return
 */
enum parser_status parse_rule_command_list(struct ast_node **ast,
                                           struct lexer *lexer)
{
    struct ast_node *ast_child = NULL;
    enum parser_status status =
        parse_rule_cmd(&ast_child, lexer); // command
    if (status != PARSER_OK)
        return PARSER_UNEXPECTED_TOKEN;

    struct ast_cmd_list_node *ast_list =
        ast_cmd_list_init(); // Create an empy AST cmdlist
    *ast = (struct ast_node *) ast_list; // attach ast_list to ast
    ast_cmd_list_add_ast(ast_list, ast_child);

    while (
        is_op(lexer_peek(lexer), OP_SEMICOLON)) // (';' simple_command)* [';']
    {
        struct token *tok = lexer_pop(lexer);
        token_free(tok);
        enum parser_status status =
            parse_rule_cmd(&ast_child, lexer);
        if (status != PARSER_OK)
            return PARSER_OK;
        ast_cmd_list_add_ast(ast_list, ast_child);
    }

    return PARSER_OK;
}
