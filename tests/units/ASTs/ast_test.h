#pragma once

#include <parser/ast_node.h>
#include <runtime.h>

struct ast_test_node
{
    struct ast_node base; // not a pointer
    // empty
};

struct ast_decrem_node
{
    struct ast_node base; // not a pointer
    int integer;
};

void ast_test_free(__attribute__((unused)) struct ast_node *ast);
void ast_test_print(__attribute__((unused)) struct ast_node *ast,
                    __attribute__((unused)) struct print_context pc);
int ast_test_exec(__attribute__((unused)) struct ast_node *ast,
                  __attribute__((unused)) struct runtime *rt);
struct ast_test_node *ast_test_init(void);

void ast_decrem_free(__attribute__((unused)) struct ast_node *ast);
void ast_decrem_print(__attribute__((unused)) struct ast_node *ast,
                      __attribute__((unused)) struct print_context pc);
int ast_decrem_exec(__attribute__((unused)) struct ast_node *ast);
struct ast_decrem_node *ast_decrem_init(void);
