#include "ast_for.h"

#include <stdio.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
void ast_for_free(struct ast_node *ast)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    // vector_apply_on_elts(ast_for->condition, &free);
    vector_destroy(ast_for->condition);
    ast_node_free(ast_for->body);
}

/**
 * @brief NOT FINISHED
 */
void ast_for_print(struct ast_node *ast, struct print_context pc)
{
    struct print_context new_pc = { pc.indent + 1 };

    struct ast_for *ast_for = (struct ast_for *)ast;
    ast_node_print_indent(pc.indent); // add indent
    printf("for {\n");
    ast_node_print_indent(pc.indent);
    char *temp = ast_for->condition->data[0];
    printf("%s\n", temp); // mandatory word
    //write(STDOUT_FILENO, temp[0], 1);
    ast_node_print_indent(pc.indent); // add indent
    printf("} in {\n");
    /*for (size_t i = 1; i < ast_for->condition->size; i++)
    {
        temp = ast_for->condition->data[i];
        printf("%s\n", temp);
    }*/
    ast_node_print_indent(pc.indent); // add indent
    printf("};\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("do {\n");
    ast_node_print_rec(ast_for->body, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("};");
}

/**
 * @brief NOT FINISHED
 */
int ast_for_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    size_t c;
    for (c = ast_for->advancement + 1; c < ast_for->condition->size; c++)
    {
        ast_node_exec(ast_for->body, rt);
    }

    return 0;
}

/**
 * @brief Initializes an AST for. Its vector condition has a size 5
 */
struct ast_for *ast_for_init()
{
    struct ast_for *new_ast =
        xmalloc(sizeof(struct ast_for)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_FOR;
    base->node_free = &ast_for_free;
    base->node_print = &ast_for_print;
    base->node_exec = &ast_for_exec;

    new_ast->condition = vector_init(5);
    new_ast->body = NULL;
    new_ast->advancement = 0;
    return new_ast;
}

/**
 * @brief Adds a new word to the vector
 */
void ast_for_add_word(struct ast_for *ast, struct token *token)
{
    if (token->type == TOKEN_WORD)
    {
        char *elt = token->data.word;
        ast->condition = vector_append(ast->condition, elt);
    }
}

void ast_for_set_body(struct ast_for *ast, struct ast_node *body)
{
    ast->body = body;
}
