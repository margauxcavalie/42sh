#pragma once

#include <lexer/operators.h>
#include <parser/ast_node.h>

struct ast_redir
{
    struct ast_node base; // not a pointer
    int fd;
    char *file;
    enum redir_type type;
    struct ast_node *child;
};

struct ast_redir *ast_redir_init();
int ast_redir_exec(struct ast_node *ast);
void ast_redir_print(struct ast_node *ast, struct print_context pc);
void ast_redir_free(struct ast_node *ast);

void ast_redir_set_child(struct ast_redir *ast, struct ast_node *child);
void ast_redir_set_file(struct ast_redir *ast, const char *file);
void ast_redir_set_fd(struct ast_redir *ast, int fd);
void ast_redir_set_type(struct ast_redir *ast, enum redir_type type);
