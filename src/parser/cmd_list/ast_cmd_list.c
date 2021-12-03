#include "ast_cmd_list.h"

#include <stdio.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
void ast_cmd_list_free(struct ast_node *ast)
{
    struct ast_cmd_list *ast_cmd_list = (struct ast_cmd_list *)ast;
    vector_apply_on_elts(ast_cmd_list->ast_list, &ast_node_free);
    vector_destroy(ast_cmd_list->ast_list);
}

/**
 * @brief Prints the content of an AST cmd_list.
 */
void ast_cmd_list_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_cmd_list *ast_cmd_list = (struct ast_cmd_list *)ast;
    struct vector *v = ast_cmd_list->ast_list;
    if (v->size == 0) // Vector is empty
        return;

    for (size_t i = 0; i < v->size - 1; i++)
    {
        ast_node_print_rec(v->data[i], pc);
        printf(";\n");
    }

    ast_node_print_rec(v->data[v->size - 1], pc);
}

int ast_cmd_list_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_cmd_list *ast_cmd_list = (struct ast_cmd_list *)ast;
    struct vector *v = ast_cmd_list->ast_list;
    if (v->size == 0) // Vector is empty
        return 0;

    if (!v || v->size == 0) // Vector is empty or non-existent
    { // usually impossible since a command list must have at least 1 command
        return 1;
    }

    for (size_t i = 0; i < v->size - 1; i++)
    {
        ast_node_exec(v->data[i], rt);
    }

    int last_return_code = ast_node_exec(v->data[v->size - 1], rt);
    return last_return_code;
}

/**
 * @brief Initializes a cmd_list AST. Its vector has a size 5
 */
struct ast_cmd_list *ast_cmd_list_init(void)
{
    struct ast_cmd_list *new_ast =
        xmalloc(sizeof(struct ast_cmd_list)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_CMD_LIST;
    base->node_free = &ast_cmd_list_free;
    base->node_print = &ast_cmd_list_print;
    base->node_exec = &ast_cmd_list_exec;

    new_ast->ast_list = vector_init(5);
    return new_ast;
}

/**
 * @brief Adds a new AST to the already existing AST.
 * @param ast_elt Newly added AST
 */
void ast_cmd_list_add_ast(struct ast_cmd_list *ast, struct ast_node *ast_elt)
{
    ast->ast_list = vector_append(ast->ast_list, ast_elt);
}
