#pragma once

#include <parser/ast_node.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct ast_subshell
{
    struct ast_node base; // not a pointer
    struct ast_node *body;
};

struct ast_subshell *ast_subshell_init();
int ast_subshell_exec(struct ast_node *ast, struct runtime *rt);
void ast_subshell_print(struct ast_node *ast, struct print_context pc);
void ast_subshell_free(struct ast_node *ast);

void ast_subshell_set_body(struct ast_subshell *ast, struct ast_node *body);
