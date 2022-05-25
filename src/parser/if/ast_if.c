#include "ast_if.h"

#include <stdio.h>
#include <utils/alloc.h>

/**
 * @brief Frees all the AST contains
 */
void ast_if_free(struct ast_node *ast)
{
    struct ast_if *ast_if = (struct ast_if *)ast;
    ast_node_free(ast_if->condition);
    ast_node_free(ast_if->if_body);
    ast_node_free(ast_if->else_body);
}

/**
 * @brief Prints the content of an AST if.
 */
void ast_if_print(struct ast_node *ast, struct print_context pc)
{
    struct print_context new_pc = { pc.indent + 1 };

    struct ast_if *ast_if = (struct ast_if *)ast;
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("if {\n");
    ast_node_print_rec(ast_if->condition, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("};\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("then {\n");
    ast_node_print_rec(ast_if->if_body, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("};");
    if (ast_if->else_body != NULL)
    {
        printf("\n");
        ast_node_print_indent(pc.indent); // ajoute l'indentation
        printf("else {\n");
        ast_node_print_rec(ast_if->else_body, new_pc);
        printf("\n");
        ast_node_print_indent(pc.indent); // ajoute l'indentation
        printf("}");
    }
}

int ast_if_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_if *ast_if = (struct ast_if *)ast;
    if (!ast_node_exec(ast_if->condition, rt)) // if it returns 0
    {
        return ast_node_exec(ast_if->if_body, rt);
    }
    if (ast_if->else_body != NULL)
    {
        return ast_node_exec(ast_if->else_body, rt);
    }
    return 0;
}

/**
 * @brief Initializes an AST if. Its vector has a size 5
 */
struct ast_if *ast_if_init()
{
    struct ast_if *new_ast =
        xmalloc(sizeof(struct ast_if)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_IF;
    base->node_free = &ast_if_free;
    base->node_print = &ast_if_print;
    base->node_exec = &ast_if_exec;

    new_ast->condition = NULL;
    new_ast->if_body = NULL;
    new_ast->else_body = NULL;
    return new_ast;
}

void ast_if_set_condition(struct ast_if *ast, struct ast_node *condition)
{
    ast->condition = condition;
}

void ast_if_set_body(struct ast_if *ast, struct ast_node *body)
{
    ast->if_body = body;
}

void ast_if_set_else(struct ast_if *ast, struct ast_node *else_body)
{
    ast->else_body = else_body;
}
