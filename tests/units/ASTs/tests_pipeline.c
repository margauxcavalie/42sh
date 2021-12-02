#include "ast_test.h"

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <parser/pipeline/ast_pipeline.h>
#include <vector/vector.h>

Test(ast_pipeline, empty)
{
    // init an empty pipeline ast
    struct ast_pipeline *ast = ast_pipeline_init();
    cr_assert_eq(ast->ast_list->size, 0);
    // print context
    struct print_context pc = { 0 };
    // test print
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    // test exec
    int res = (*(ast->base.node_exec))((struct ast_node *)ast);
    cr_assert_eq(res, 0);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
}

Test(ast_pipeline, one_ast)
{
    // redirect stdout
    cr_redirect_stdout();
    // init an empty pipeline ast
    struct ast_pipeline *ast = ast_pipeline_init();
    // add test ast
    struct ast_node *elt = (struct ast_node *)ast_test_init();
    ast_pipeline_add_ast(ast, elt);
    cr_assert_eq(ast->ast_list->size, 1);
    cr_assert_eq(ast->ast_list->data[0], (void *)elt);

    // print context
    struct print_context pc = { 0 };
    // test print
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str("ast_test: print");

    // test exec
    //int res = (*(ast->base.node_exec))((struct ast_node *)ast);
    //cr_assert_eq(res, 666);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
}