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
static void ast_cmd_list_free(struct ast_node *ast)
{
    struct ast_cmd_list_node *ast_cmd_list = (struct ast_cmd_list_node *)ast;
    vector_apply_on_elts(ast_cmd_list->ast_list, &ast_node_free);
    vector_destroy(ast_cmd_list->ast_list);
}

/**
 * @brief Prints the content of an AST cmd_list.
 */
static void ast_cmd_list_print(struct ast_node *ast)
{
    struct ast_cmd_list_node *ast_cmd_list = (struct ast_cmd_list_node *)ast;
    struct vector *v = ast_cmd_list->ast_list;
    if (!v || v->size == 0) // Vector is empty or non-existent
    { // usually impossible since a command list must have at least 1 command
        printf(
            "\nERROR: command list without commands\n"); 
        return;
    }

    for (size_t i = 0; i < v->size - 1; i++)
    {
        ast_node_print_rec(v->data[i]);
        printf("; ");
    }

    ast_node_print_rec(v->data[v->size - 1]);
}

/**
 * @brief Initializes a cmd_list AST. Its vector has a size 5
 */
static struct ast_cmd_list_node *ast_cmd_list_init(void)
{
    struct ast_cmd_list_node *new_ast = xmalloc(
        sizeof(struct ast_cmd_list_node)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_CMD_LIST;
    base->node_free = &ast_cmd_list_free;
    base->node_print = &ast_cmd_list_print;
    // base->node_exec = TODO

    new_ast->ast_list = vector_init(5);
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
 * list: simple_command (';' simple_command)* [';']
 * @return
 */
enum parser_status parse_rule_command_list(struct ast_node **ast,
                                           struct lexer *lexer)
{
    struct ast_node *ast_child = NULL;
    enum parser_status status =
        parse_rule_simple_command(&ast_child, lexer); // simple_command
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
            parse_rule_simple_command(&ast_child, lexer);
        if (status != PARSER_OK)
            return PARSER_OK;
        ast_cmd_list_add_ast(ast_list, ast_child);
    }

    return PARSER_OK;
}