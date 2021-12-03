#include "ast_redir.h"

#include <stdio.h>
#include <fcntl.h>
#include <utils/alloc.h>

/**
 * @brief Frees all the AST contains
 */
void ast_redir_free(struct ast_node *ast)
{
    struct ast_redir *ast_redir = (struct ast_redir *)ast;
    ast_node_free(ast_redir->child);
    free(ast_redir->file);
}

/**
 * @brief Prints the content of an AST if.
 */
void ast_redir_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_redir *ast_redir = (struct ast_redir *)ast;
    ast_node_print_rec(ast_redir->child, pc);
    enum redir_type type = ast_redir->type;
    char redir_op[3];
    if (type == REDIR_GREAT)
        strcpy(redir_op, ">");
    else if (type == REDIR_LESS)
        strcpy(redir_op, "<");
    else if (type == REDIR_GREATAND)
        strcpy(redir_op, ">&");
    else if (type == REDIR_LESSAND)
        strcpy(redir_op, "<&");
    else if (type == REDIR_DGREAT)
        strcpy(redir_op, ">>");
    else if (type == REDIR_LESSGREAT)
        strcpy(redir_op, "<>");
    else if (type == REDIR_CLOBBER)
        strcpy(redir_op, "<|");

    printf(" %d%s %s", ast_redir->fd, redir_op, ast_redir->file);
}

int ast_redir_exec(struct ast_node *ast)
{
    struct ast_redir *ast_redir = (struct ast_redir *)ast;
    if (ast_redir->type != REDIR_GREAT)
        return ast_node_exec(ast_redir->child);
    // its '>'
    ast_redir->fd = 1; // stdout
    int fd = open(ast_redir->file, O_WRONLY | O_CREAT);
    // TODO on error
    int saved_stdout = dup(ast_redir->fd);
    dup2(fd, ast_redir->fd);

    int status = ast_node_exec(ast_redir->child);
    fflush(stdout);

    close(fd);
    dup2(saved_stdout, ast_redir->fd);
    close(saved_stdout);
    return status;
}

/**
 * @brief Initializes a redir AST. Its vector has a size 5
 */
struct ast_redir *ast_redir_init()
{
    struct ast_redir *new_ast =
        xmalloc(sizeof(struct ast_redir)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_REDIR;
    base->node_free = &ast_redir_free;
    base->node_print = &ast_redir_print;
    base->node_exec = &ast_redir_exec;

    new_ast->child = NULL;
    new_ast->file = NULL;
    new_ast->fd = 1;
    return new_ast;
}

void ast_redir_set_fd(struct ast_redir *ast, int fd)
{
    ast->fd = fd;
}

void ast_redir_set_file(struct ast_redir *ast, const char *file)
{
    char *new = zalloc(sizeof(char) * (strlen(file) + 1));
    strcpy(new, file);
    ast->file = new;
}

void ast_redir_set_type(struct ast_redir *ast, enum redir_type type)
{
    ast->type = type;
}

void ast_redir_set_child(struct ast_redir *ast, struct ast_node *child)
{
    ast->child = child;
}

