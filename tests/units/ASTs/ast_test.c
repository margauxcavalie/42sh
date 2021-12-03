#include "ast_test.h"

#include <stdio.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief This free function do nothing
 */
void ast_test_free(__attribute__((unused)) struct ast_node *ast)
{
    return;
}

/**
 * @brief Prints the content of an AST tests.
 */
void ast_test_print(__attribute__((unused)) struct ast_node *ast,
                    __attribute__((unused)) struct print_context pc)
{
    ast_node_print_indent(pc.indent);
    printf("ast_test: print");
}

int ast_test_exec(__attribute__((unused)) struct ast_node *ast,
                  __attribute__((unused)) struct runtime *rt)
{
    return 666;
}

/**
 * @brief Initializes a test AST
 */
struct ast_test_node *ast_test_init(void)
{
    struct ast_test_node *new_ast =
        xmalloc(sizeof(struct ast_test_node)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_TEST;
    base->node_free = &ast_test_free;
    base->node_print = &ast_test_print;
    base->node_exec = &ast_test_exec;

    return new_ast;
}

/**
 * @brief This free function do nothing
 */
void ast_decrem_free(__attribute__((unused)) struct ast_node *ast)
{
    return;
}

/**
 * @brief Prints the content of an AST tests.
 */
void ast_decrem_print(__attribute__((unused)) struct ast_node *ast,
                      __attribute__((unused)) struct print_context pc)
{
    ast_node_print_indent(pc.indent);
    printf("ast_test_decrem: print");
}

int ast_decrem_exec(__attribute__((unused)) struct ast_node *ast)
{
    struct ast_decrem_node *decrem = (struct ast_decrem_node *)ast;
    return (((decrem->integer)-- == 0) ? (1) : (0));
}

/**
 * @brief Initializes a test_decrement AST
 */
struct ast_decrem_node *ast_decrem_init(void)
{
    struct ast_decrem_node *new_ast = xmalloc(
        sizeof(struct ast_decrem_node)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_DECREM;
    base->node_free = &ast_decrem_free;
    base->node_print = &ast_decrem_print;
    base->node_exec = &ast_decrem_exec;

    return new_ast;
}
