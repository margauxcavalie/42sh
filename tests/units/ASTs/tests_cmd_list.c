#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <parser/cmd_list/ast_cmd_list.h>
#include <vector/vector.h>

#include "ast_test.h" // test ast

/**
 * We suppose the "test ast" always correct
 */

/**
 *
 * This file check the behavior of the structure ast "cmd_list"
 *
 * Check:
 *  - print
 *  - exec
 *  - free
 *
 */

Test(ast_cmd_list, empty)
{
    // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty cmd_list ast
    struct ast_cmd_list *ast = ast_cmd_list_init();
    cr_assert_eq(ast->ast_list->size, 0);
    // print context
    struct print_context pc = { 0 };
    // test print
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    // test exec
    int res = (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    cr_assert_eq(res, 0);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_cmd_list, one_ast)
{
    // Runtime struct
    struct runtime *rt = runtime_init();
    // redirect stdout
    cr_redirect_stdout();
    // init an empty cmd_list ast
    struct ast_cmd_list *ast = ast_cmd_list_init();
    // add test ast
    struct ast_node *elt = (struct ast_node *)ast_test_init();
    ast_cmd_list_add_ast(ast, elt);
    cr_assert_eq(ast->ast_list->size, 1);
    cr_assert_eq(ast->ast_list->data[0], (void *)elt);

    // print context
    struct print_context pc = { 0 };
    // test print
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str("ast_test: print");

    // test exec
    int res = (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    cr_assert_eq(res, 666);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_cmd_list, two_ast)
{
    // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty cmd_list ast
    struct ast_cmd_list *ast = ast_cmd_list_init();
    // add test ast
    struct ast_node *elt1 = (struct ast_node *)ast_test_init();
    ast_cmd_list_add_ast(ast, elt1);
    cr_assert_eq(ast->ast_list->size, 1);
    cr_assert_eq(ast->ast_list->data[0], (void *)elt1);

    struct ast_node *elt2 = (struct ast_node *)ast_test_init();
    ast_cmd_list_add_ast(ast, elt2);
    cr_assert_eq(ast->ast_list->size, 2);
    cr_assert_eq(ast->ast_list->data[1], (void *)elt2);

    // redirect stdout
    cr_redirect_stdout();
    // print context
    struct print_context pc = { 0 };
    // test print
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str("ast_test: print;\nast_test: print");

    // test exec
    int res = (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    cr_assert_eq(res, 666);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}
