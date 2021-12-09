#include "ast_for.h"

#include <err.h>
#include <expansion/expansion.h>
#include <stdio.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
void ast_for_free(struct ast_node *ast)
{
    struct ast_for *ast_for = (struct ast_for *)ast;
    if (ast_for->var_name)
        free(ast_for->var_name);
    vector_apply_on_elts(ast_for->list, &free);
    vector_destroy(ast_for->list);
    ast_node_free(ast_for->body);
}

/**
 * @brief Prints the content of the AST
 */
void ast_for_print(struct ast_node *ast, struct print_context pc)
{
    struct print_context new_pc = { pc.indent + 1 };

    struct ast_for *ast_for = (struct ast_for *)ast;
    ast_node_print_indent(pc.indent); // add indent
    printf("for { ");
    ast_node_print_indent(pc.indent);
    printf("%s ", ast_for->var_name); // mandatory word
    ast_node_print_indent(pc.indent); // add indent
    printf("} in { ");
    for (size_t i = 0; i < ast_for->list->size; i++)
    {
        char *temp = ast_for->list->data[i];
        printf("%s ", temp);
    }
    ast_node_print_indent(pc.indent); // add indent
    printf("};\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("do {\n");
    ast_node_print_rec(ast_for->body, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("};");
}

static struct vector *expands_for_list(struct ast_for *ast, struct runtime *rt)
{
    struct vector *new = vector_init(ast->list->size);
    for (size_t i = 0; i < ast->list->size; i++)
    {
        struct vector *expanded_word = expands_word(ast->list->data[i], rt);
        if (expanded_word == NULL)
        {
            warnx("Expansion: unmacthed quotes (tmp)");
            vector_destroy(new);
            return NULL;
        }
        for (size_t j = 0; j < expanded_word->size; j++)
        {
            vector_append(new, expanded_word->data[j]);
        }
        vector_destroy(expanded_word); // do not destroy items
    }
    return new;
}

/**
 * @brief Executes the body of the for
 */
int ast_for_exec(struct ast_node *ast, struct runtime *rt)
{
    rt->loops_count += 1;
    struct ast_for *ast_for = (struct ast_for *)ast;

    struct vector *expanded_list = expands_for_list(ast_for, rt);
    if (expanded_list == NULL)
        return 2;

    for (size_t c = 0; c < expanded_list->size; c++)
    {
        // SET variable
        ast_node_exec(ast_for->body, rt);
        if (rt->loops_to_break != 0)
            break;
        rt->encountered_continue = false;
    }

    vector_apply_on_elts(expanded_list, &free);
    vector_destroy(expanded_list);

    if (rt->loops_to_break > 0)
        rt->loops_to_break -= 1;
    rt->loops_count -= 1;
    if (rt->encountered_exit == true)
        return rt->last_status;
    return 0;
}

/**
 * @brief Initializes an AST for. Its vector condition has a size 5
 */
struct ast_for *ast_for_init()
{
    struct ast_for *new_ast =
        xmalloc(sizeof(struct ast_for)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_FOR;
    base->node_free = &ast_for_free;
    base->node_print = &ast_for_print;
    base->node_exec = &ast_for_exec;

    new_ast->list = vector_init(5);
    new_ast->body = NULL;
    new_ast->var_name = NULL;
    return new_ast;
}

/**
 * @brief Adds a new word to the vector
 */
void ast_for_add_word(struct ast_for *ast, char *word)
{
    char *elt = strdup(word);
    ast->list = vector_append(ast->list, elt);
}

void ast_for_set_body(struct ast_for *ast, struct ast_node *body)
{
    ast->body = body;
}

void ast_for_set_var_name(struct ast_for *ast, char *name)
{
    ast->var_name = strdup(name);
}
