#pragma once

#include <lexer/lexer.h>
#include <stddef.h>

enum ast_type
{
    AST_TEST, // For the units tests
    AST_SIMPLE_CMD,
    AST_CMD_LIST,
    AST_IF,
    AST_REDIR,
};

struct print_context
{
    int indent;
};

/**
 * @brief Base struct for every AST node
 *
 */
struct ast_node
{
    enum ast_type type;
    // pointeur de fonction pour free
    void (*node_free)(struct ast_node *node);
    // pointeur de fonction pour print
    void (*node_print)(struct ast_node *node, struct print_context pc);
    // pointeur de fonction pour free
    int (*node_exec)(struct ast_node *node);
};

void ast_node_free(void *ptr);
void ast_node_free_detach(struct ast_node **ast);

void ast_node_print_rec(void *ptr, struct print_context pc);
void ast_node_print(void *ptr);
void ast_node_print_indent(int indent);

int ast_node_exec(void *ptr);
