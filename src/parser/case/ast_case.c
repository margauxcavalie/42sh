#include "ast_case.h"

#include <err.h>
#include <expansion/expansion.h>
#include <stdbool.h>
#include <stdio.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
void ast_case_free(struct ast_node *ast)
{
    struct ast_case *ast_case = (struct ast_case *)ast;
    if (ast_case->reference)
        free(ast_case->reference);

    for (size_t i = 0; i < ast_case->bodies->size; i++)
    {
        ast_node_free(ast_case->bodies->data[i]);
    }

    for (size_t i = 0; i < ast_case->patterns->size; i++)
    {
        struct vector *match_item = ast_case->patterns->data[i];
        for (size_t j = 0; j < match_item->size; j++)
        {
            free(match_item->data[j]);
        }

        vector_destroy(match_item);
    }

    vector_destroy(ast_case->bodies);
    vector_destroy(ast_case->patterns);
}

/**
 * @brief Prints the content of the AST
 */
void ast_case_print(struct ast_node *ast, struct print_context pc)
{
    struct ast_case *ast_case = (struct ast_case *)ast;
    ast_node_print_indent(pc.indent); // add indent
    printf("case { ");
    ast_node_print_indent(pc.indent);
    printf("%s ", ast_case->reference); // mandatory word
    ast_node_print_indent(pc.indent); // add indent
    printf("} in { ");
    printf("trucs\n");
    ast_node_print_indent(pc.indent); // add indent
    printf("};\n");
}

static char *expand_pattern(char *pattern, struct runtime *rt)
{
    struct vector *expanded_word = expands_word(pattern, rt);
    if (!expanded_word)
    {
        warnx("Expansion: unmacthed quotes (tmp)\n");
        return NULL;
    }

    if (expanded_word->size > 1)
    {
        warnx("case does not accept multiple expands\n");
        vector_apply_on_elts(expanded_word, &free);
        vector_destroy(expanded_word);
        return NULL;
    }

    char *expanded = expanded_word->data[0];
    vector_destroy(expanded_word); // do not destroy items
    return expanded;
}

static char *expand_reference(struct ast_case *ast, struct runtime *rt)
{
    struct vector *expanded_word = expands_word(ast->reference, rt);
    if (!expanded_word)
    {
        warnx("Expansion: unmacthed quotes (tmp)\n");
        return NULL;
    }

    if (expanded_word->size > 1)
    {
        warnx("case does not accept multiple expands\n");
        vector_apply_on_elts(expanded_word, &free);
        vector_destroy(expanded_word);
        return NULL;
    }

    char *expanded = expanded_word->data[0];
    vector_destroy(expanded_word); // do not destroy items
    return expanded;
}

/**
 * @brief Executes the body of the case
 */
int ast_case_exec(struct ast_node *ast, struct runtime *rt)
{
    struct ast_case *ast_case = (struct ast_case *)ast;
    bool executed = false;

    char *new_reference = expand_reference(ast_case, rt);
    if (!new_reference)
        return 2;

    for (size_t c = 0; c < ast_case->patterns->size; c++)
    {
        struct vector *item_match = ast_case->patterns->data[c];
        for (size_t i = 0; i < item_match->size; i++)
        {
            char *pattern = expand_pattern(item_match->data[i], rt);
            if (!pattern)
                continue;
            if (!(strcmp(new_reference, pattern)))
            {
                free(pattern);
                executed = true;
                break;
            }
            free(pattern);
        }

        if (executed)
        {
            ast_node_exec(ast_case->bodies->data[c], rt);
            break;
        }
    }

    free(new_reference);
    return ((executed) ? (rt->last_status) : (0));
}

/**
 * @brief Initializes an AST case. Its vectors have a size 5
 */
struct ast_case *ast_case_init()
{
    struct ast_case *new_ast =
        xmalloc(sizeof(struct ast_case)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_CASE;
    base->node_free = &ast_case_free;
    base->node_print = &ast_case_print;
    base->node_exec = &ast_case_exec;

    new_ast->patterns = vector_init(5);
    new_ast->bodies = vector_init(5);
    new_ast->reference = NULL;
    return new_ast;
}

void ast_case_set_reference(struct ast_case *ast, char *name)
{
    ast->reference = name;
}

void ast_case_add_pattern(struct ast_case *ast, struct vector *pattern)
{
    vector_append(ast->patterns, pattern);
}

void ast_case_add_body(struct ast_case *ast, struct ast_node *new_ast)
{
    vector_append(ast->bodies, new_ast);
}
