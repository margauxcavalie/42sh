#pragma once

#include <lexer/lexer.h>
#include <stddef.h>

enum ast_type
{
    AST_SIMPLE_CMD,
    AST_CMD_LIST,
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
    void (*node_print)(struct ast_node *node);
    // pointeur de fonction pour free
    int (*node_exec)(struct ast_node *node);
};

void ast_node_free(void *ptr);
void ast_node_print_rec(void *ptr);
void ast_node_print(void *ptr);
int ast_node_exec(void *ptr);
