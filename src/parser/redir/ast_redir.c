#include "ast_redir.h"

#include <err.h>
#include <expansion/expansion.h>
#include <stdio.h>
#include <utils/alloc.h>
#include <vector/vector.h>

#include "redir_fd.h"

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
        strcpy(redir_op, ">|");

    printf(" %d%s %s", ast_redir->fd, redir_op, ast_redir->file);
}

static char *expands_redir_path(struct ast_redir *ast, struct runtime *rt)
{
    struct vector *expanded_word = expands_word(ast->file, rt);
    if (expanded_word == NULL)
    {
        warnx("Expansion: unmacthed quotes (tmp)");
        return NULL;
    }
    if (expanded_word->size != 1)
    {
        warnx("%s: ambiguous redirect", ast->file);
        vector_apply_on_elts(expanded_word, &free);
        vector_destroy(expanded_word);
        return NULL;
    }
    char *new = expanded_word->data[0];
    vector_destroy(expanded_word); // do not destroy items
    return new;
}

int ast_redir_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_redir *ast_redir = (struct ast_redir *)ast;

    char *path_expanded = expands_redir_path(ast_redir, rt);
    if (path_expanded == NULL)
        return 2;

    int saved_fd;
    int file_fd =
        redir_in_out(ast_redir, path_expanded, &saved_fd, false); // TODO
    if (file_fd == -1)
    {
        free(path_expanded);
        return 2;
    }

    int status = ast_node_exec(ast_redir->child, rt);
    fflush(stdout);

    dup2(saved_fd, ast_redir->fd);
    close(saved_fd);
    close(file_fd);
    free(path_expanded);
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
