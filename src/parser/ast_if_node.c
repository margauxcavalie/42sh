#include <parser/ast_cmd_list_node.h>
#include <parser/ast_if_node.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils/alloc.h>
#include <vector/vector.h>

/**
 * @brief Frees all the AST contains
 */
static void ast_if_free(struct ast_node *ast)
{
    struct ast_if_node *ast_if = (struct ast_if_node *)ast;
    ast_node_free(ast_if->condition);
    ast_node_free(ast_if->if_body);
    ast_node_free(ast_if->else_body);
}

/**
 * @brief Prints the content of an AST if.
 */
static void ast_if_print(struct ast_node *ast, struct print_context pc)
{
    struct print_context new_pc = { pc.indent + 1 };

    struct ast_if_node *ast_if = (struct ast_if_node *)ast;
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("if {\n");
    ast_node_print_rec(ast_if->condition, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("};\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("then {\n");
    ast_node_print_rec(ast_if->if_body, new_pc);
    printf("\n");
    ast_node_print_indent(pc.indent); // ajoute l'indentation
    printf("};");
    if (ast_if->else_body != NULL)
    {
        printf("\n");
        ast_node_print_indent(pc.indent); // ajoute l'indentation
        printf("else {\n");
        ast_node_print_rec(ast_if->else_body, new_pc);
        printf("\n");
        ast_node_print_indent(pc.indent); // ajoute l'indentation
        printf("};\n");
    }
}

static int ast_if_exec(struct ast_node *ast)
{
    struct ast_if_node *ast_if = (struct ast_if_node *)ast;
    if (!ast_node_exec(ast_if->condition)) // if it returns 0
    {
        return ast_node_exec(ast_if->if_body);
    }
    if (ast_if->else_body != NULL)
    {
        return ast_node_exec(ast_if->else_body);
    }
    return 0;
}

/**
 * @brief Initializes a cmd_list AST. Its vector has a size 5
 */
static struct ast_if_node *ast_if_init()
{
    struct ast_if_node *new_ast =
        xmalloc(sizeof(struct ast_if_node)); // expand (unique of each types)
    struct ast_node *base = (struct ast_node *)new_ast; // reduce (common)

    // Set common properties
    base->type = AST_IF;
    base->node_free = &ast_if_free;
    base->node_print = &ast_if_print;
    base->node_exec = &ast_if_exec;

    new_ast->condition = NULL;
    new_ast->if_body = NULL;
    new_ast->else_body = NULL;
    return new_ast;
}

static void ast_if_set_condition(struct ast_if_node *ast,
                                 struct ast_node *condition)
{
    ast->condition = condition;
}

static void ast_if_set_body(struct ast_if_node *ast, struct ast_node *body)
{
    ast->if_body = body;
}

static void ast_if_set_else(struct ast_if_node *ast, struct ast_node *else_body)
{
    ast->else_body = else_body;
}

/**
 * @brief (temporary version)
 * else_clause: ELSE list
 *            | ELIF list THEN list [else_clause]
 * @return
 */
static enum parser_status parse_rule_else_clause(struct ast_node **ast,
                                                 struct lexer *lexer)
{
    if (is_rw(lexer_peek(lexer), RW_ELIF) == true)
    {
        struct ast_if_node *ast_if = ast_if_init();
        *ast = (struct ast_node *)ast_if; // attach ast_if to ast

        token_free(lexer_pop(lexer));
        struct ast_node *ast_elif_condition = NULL;
        struct ast_node *ast_elif_body = NULL;
        struct ast_node *ast_elif_else = NULL;

        enum parser_status status =
            parse_rule_command_list(&ast_elif_condition, lexer);
        ast_if_set_condition(ast_if,
                             ast_elif_condition); // Set condition_body

        if (status != PARSER_OK)
            goto error_elif;

        if (is_rw(lexer_peek(lexer), RW_THEN) == false)
            goto error_elif;
        token_free(lexer_pop(lexer));

        status = parse_rule_command_list(&ast_elif_body, lexer);
        ast_if_set_body(ast_if,
                        ast_elif_body); // Set if_body

        if (status != PARSER_OK)
            goto error_elif;

        parse_rule_else_clause(&ast_elif_else, lexer);
        ast_if_set_else(ast_if,
                        ast_elif_else); // Set else_body

        return PARSER_OK;
    error_elif:
        ast_node_free_detach(ast);
        return PARSER_UNEXPECTED_TOKEN;
    }

    if (is_rw(lexer_peek(lexer), RW_ELSE) == true)
    {
        token_free(lexer_pop(lexer));
        enum parser_status status = parse_rule_command_list(ast, lexer);
        if (status != PARSER_OK)
            return PARSER_UNEXPECTED_TOKEN;
        return PARSER_OK;
    }

    return PARSER_UNEXPECTED_TOKEN;
}

/**
 * @brief (temporary version)
 * rule_if: IF list THEN list [else_clause] FI
 * @return
 */
enum parser_status parse_rule_if(struct ast_node **ast, struct lexer *lexer)
{
    struct ast_if_node *ast_if = ast_if_init();
    *ast = (struct ast_node *)ast_if; // attach ast_if to ast

    struct ast_node *ast_if_condition = NULL;
    struct ast_node *ast_if_body = NULL;
    struct ast_node *ast_else_body = NULL;

    if (is_rw(lexer_peek(lexer), RW_IF) == false)
        goto error;
    token_free(lexer_pop(lexer));

    enum parser_status status =
        parse_rule_command_list(&ast_if_condition, lexer);
    ast_if_set_condition(ast_if, ast_if_condition); // Set the condition_body

    if (status != PARSER_OK)
        goto error;

    if (is_rw(lexer_peek(lexer), RW_THEN) == false)
        goto error;
    token_free(lexer_pop(lexer));

    status = parse_rule_command_list(&ast_if_body, lexer);
    ast_if_set_body(ast_if, ast_if_body); // Set the if_body

    if (status != PARSER_OK)
        goto error;

    parse_rule_else_clause(&ast_else_body, lexer);
    ast_if_set_else(ast_if, ast_else_body); // Set the else_body

    if (is_rw(lexer_peek(lexer), RW_FI) == false)
        goto error;
    token_free(lexer_pop(lexer));

    return PARSER_OK;
error:
    ast_node_free_detach(ast);
    return PARSER_UNEXPECTED_TOKEN;
}