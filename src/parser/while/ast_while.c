#include "ast_while.h"

#include <stdio.h>
#include <utils/alloc.h>

/**
 * @brief Frees all the AST contains
 */
void ast_while_free(struct ast_node *ast)
{
    struct ast_while *ast_while = (struct ast_while *)ast;
    ast_node_free(ast_while->condition);
    ast_node_free(ast_while->body);
}

/**
 * @brief Prints the content of an AST while.
 */
void ast_while_print(struct ast_node *ast, struct print_context pc)
{
    struct print_context new_pc = { pc.indent + 1 };

    struct ast_while *ast_while = (struct ast_while *)ast;
    ast_node_print_indent(pc.indent); // add indent
    printf("while {\n");
    ast_node_print_rec(ast_while->condition, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("};\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("do {\n");
    ast_node_print_rec(ast_while->body, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("};");
}

/**
 * @brief Prints the content of an AST until.
 */
void ast_until_print(struct ast_node *ast, struct print_context pc)
{
    struct print_context new_pc = { pc.indent + 1 };

    struct ast_while *ast_while = (struct ast_while *)ast;
    ast_node_print_indent(pc.indent); // add indent
    printf("until {\n");
    ast_node_print_rec(ast_while->condition, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("};\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("do {\n");
    ast_node_print_rec(ast_while->body, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("};");
}

int ast_while_exec(struct ast_node *ast)
{
    struct ast_while *ast_while = (struct ast_while *)ast;
    while (!ast_node_exec(ast_while->condition)) // if it returns 0
        ast_node_exec(ast_while->body);

    return 0;
}

int ast_until_exec(struct ast_node *ast)
{
    struct ast_while *ast_while = (struct ast_while *)ast;
    while (ast_node_exec(ast_while->condition)) // if it returns 0
        ast_node_exec(ast_while->body);

    return 0;
}

/**
 * @brief Initializes an AST while. Its vector has a size 5
 */
struct ast_while *ast_while_init()
{
    struct ast_while *new_ast = xmalloc(sizeof(struct ast_while));
    struct ast_node *base = (struct ast_node *)new_ast;

    // Set common properties
    base->type = AST_WHILE;
    base->node_free = &ast_while_free;
    base->node_print = &ast_while_print;
    base->node_exec = &ast_while_exec;

    new_ast->condition = NULL;
    new_ast->body = NULL;
    return new_ast;
}

/**
 * @brief Initializes an AST until. Its vector has a size 5
 */
struct ast_while *ast_until_init()
{
    struct ast_while *new_ast = xmalloc(sizeof(struct ast_while));
    struct ast_node *base = (struct ast_node *)new_ast;

    // Set common properties
    base->type = AST_WHILE;
    base->node_free = &ast_while_free;
    base->node_print = &ast_until_print;
    base->node_exec = &ast_until_exec;

    new_ast->condition = NULL;
    new_ast->body = NULL;
    return new_ast;
}

void ast_while_set_condition(struct ast_while *ast, struct ast_node *condition)
{
    ast->condition = condition;
}

void ast_while_set_body(struct ast_while *ast, struct ast_node *body)
{
    ast->body = body;
}
