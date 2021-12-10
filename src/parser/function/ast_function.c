#include "ast_function.h"

#include <hash_map_function/hash_map_function.h>
#include <stdio.h>
#include <utils/alloc.h>

void ast_function_free(struct ast_node *ast)
{
    struct ast_function *ast_function = (struct ast_function *)ast;
    ast_node_free(ast_function->child);
    free(ast_function->name);
}

void ast_function_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_function *ast_function = (struct ast_function *)ast;
    printf("%s() { ", ast_function->name);
    ast_node_print_rec(ast_function->child, pc);
    printf(" }");
}

int ast_function_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_function *ast_function = (struct ast_function *)ast;

    // store function in hash table
    bool res = hash_map_func_insert(rt->functions, ast_function->name, ast,
                                    &ast_function_free);
    return (res == true) ? 0 : 1;
}

struct ast_function *ast_function_init(char *name)
{
    struct ast_function *new_ast =
        xmalloc(sizeof(struct ast_function)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_FUNCTION;
    base->node_free = &ast_function_free;
    base->node_print = &ast_function_print;
    base->node_exec = &ast_function_exec;

    new_ast->child = NULL;
    new_ast->name = zalloc(sizeof(char) * 1024);
    strcpy(new_ast->name, name);
    return new_ast;
}