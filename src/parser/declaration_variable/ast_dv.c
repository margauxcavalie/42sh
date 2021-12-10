#include "ast_dv.h"

#include <err.h>
#include <expansion/expansion.h>
#include <stdio.h>
#include <string.h>
#include <utils/alloc.h>
#include <utils/vec.h>
#include <vector/vector.h>

/**
 * @brief expands the value contained in the dv
 *
 * @param ast
 * @param rt
 * @return char*
 */
static char *expands_value(struct ast_dv *ast, struct runtime *rt)
{
    struct vector *expanded_value = expands_word(ast->value, rt);
    if (expanded_value == NULL)
    {
        warnx("Expansion: unmacthed quotes (tmp)");
        return NULL;
    }

    char *new = expanded_value->data[0];
    vector_destroy(expanded_value); // do not destroy items
    return new;
}

/**
 * @brief execution of the ast_dv
 *
 * @param ast
 * @param rt
 * @return int
 */
int ast_dv_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_dv *ast_dv = (struct ast_dv *)ast;

    char *value_expanded = expands_value(ast_dv, rt);
    if (value_expanded == NULL)
        return 2;
    char *hash_key = zalloc(sizeof(char) * (strlen(ast_dv->key) + 1));
    strcpy(hash_key, ast_dv->key);
    bool res = var_hash_map_insert(rt->variables, hash_key, value_expanded);
    if (res == false)
    {
        free(value_expanded);
        return 2;
    }
    int status = ast_node_exec(ast_dv->child, rt);
    fflush(stdout);
    return status;
}

/**
 * @brief print the content of an ast_dv
 *
 * @param ast
 * @param pc
 */
void ast_dv_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_dv *ast_dv = (struct ast_dv *)ast;
    printf("%s=%s ", ast_dv->key, ast_dv->value);
    ast_node_print_rec(ast_dv->child, pc);
}
/**
 * @brief Frees all the AST contains
 */
void ast_dv_free(struct ast_node *ast)
{
    struct ast_dv *ast_dv = (struct ast_dv *)ast;
    ast_node_free(ast_dv->child);
    free(ast_dv->key);
    free(ast_dv->value);
}

/**
 * @brief Initializes a declaration variable ast
 */
struct ast_dv *ast_dv_init()
{
    struct ast_dv *new_ast =
        xmalloc(sizeof(struct ast_dv)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_DV;
    base->node_free = &ast_dv_free;
    base->node_print = &ast_dv_print;
    base->node_exec = &ast_dv_exec;

    new_ast->child = NULL;
    new_ast->value = NULL;
    new_ast->key = NULL;
    return new_ast;
}

void ast_dv_set_child(struct ast_dv *ast, struct ast_node *child)
{
    ast->child = child;
}

void ast_dv_set_key_value_pair(struct ast_dv *ast, char *word)
{
    struct vec *vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    size_t i = 0;
    for (; word[i] && word[i] != '='; i++)
    {
        vec_push(vec, word[i]);
    }
    ast->key = vec_cstring(vec);
    free(vec);
    vec = xmalloc(sizeof(struct vec));
    vec_init(vec);
    i += 1;
    for (; word[i] && word[i]; i++)
    {
        vec_push(vec, word[i]);
    }
    ast->value = vec_cstring(vec);
    free(vec);
    // printf("%s\n", ast->key);
    // printf("%s\n", ast->value);
}
