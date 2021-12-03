#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <parser/simple_cmd/ast_simple_cmd.h>
#include <parser/while_until/ast_while_until.h>
#include <vector/vector.h>

#include "ast_test.h"

// AST WHILE //
//
Test(ast_while, empty)
{
     // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_while_until *ast = ast_while_init();
    struct print_context pc = { 0 };

    // test print
    cr_redirect_stdout();
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str("while {\n\n};\ndo {\n\n};");

    // test exec
    int res = (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    cr_assert_eq(res, 0);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_while, print_test)
{
     // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_while_until *ast = ast_while_init();
    struct print_context pc = { 0 };

    struct ast_decrem_node *condition = ast_decrem_init();
    struct ast_test_node *body = ast_test_init();
    ast_while_until_set_condition(ast, &(condition->base));
    ast_while_until_set_body(ast, &(body->base));

    // test print
    cr_redirect_stdout();
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str("while {\n    ast_test_decrem: print\n};\ndo {\n   "
                            " ast_test: print\n};");

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_while, print_test_concret)
{
     // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_while_until *ast = ast_while_init();

    struct ast_decrem_node *condition = ast_decrem_init();
    condition->integer = 3;
    struct ast_simple_cmd *body = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body, "echo");
    ast_simple_cmd_add_param(body, "teckel");

    ast_while_until_set_condition(ast, &(condition->base));
    ast_while_until_set_body(ast, &(body->base));

    // test print
    cr_redirect_stdout();
    struct print_context pc = { 0 };
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "while {\n    ast_test_decrem: print\n};\ndo {\n    echo teckel\n};");

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_while, exec_test_concret)
{
     // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_while_until *ast = ast_while_init();

    struct ast_decrem_node *condition = ast_decrem_init();
    condition->integer = 3;
    struct ast_simple_cmd *body = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body, "echo");
    ast_simple_cmd_add_param(body, "teckel");

    ast_while_until_set_condition(ast, &(condition->base));
    ast_while_until_set_body(ast, &(body->base));

    // test exec
    cr_redirect_stdout();
    (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    fflush(stdout);
    cr_assert_stdout_eq_str("teckel\nteckel\nteckel\n");

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}
//
//
//

// AST UNTIL //
//
Test(ast_until, empty)
{
     // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST until
    struct ast_while_until *ast = ast_until_init();
    struct print_context pc = { 0 };

    // test print
    cr_redirect_stdout();
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str("until {\n\n};\ndo {\n\n};");

    // test exec
    int res = (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    cr_assert_eq(res, 0);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_until, print_test_concret)
{
     // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_while_until *ast = ast_until_init();

    struct ast_decrem_node *condition = ast_decrem_init();
    condition->integer = 0;
    struct ast_simple_cmd *body = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body, "echo");
    ast_simple_cmd_add_param(body, "teckel");

    ast_while_until_set_condition(ast, &(condition->base));
    ast_while_until_set_body(ast, &(body->base));

    // test print
    cr_redirect_stdout();
    struct print_context pc = { 0 };
    (*(ast->base.node_print))((struct ast_node *)ast, pc);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "until {\n    ast_test_decrem: print\n};\ndo {\n    echo teckel\n};");

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}

Test(ast_until, exec_test_concret)
{
     // Runtime struct
    struct runtime *rt = runtime_init();
    // init an empty AST while
    struct ast_while_until *ast = ast_until_init();

    struct ast_decrem_node *condition = ast_decrem_init();
    condition->integer = 0;
    struct ast_simple_cmd *body = ast_simple_cmd_init();
    ast_simple_cmd_add_param(body, "echo");
    ast_simple_cmd_add_param(body, "husky");

    ast_while_until_set_condition(ast, &(condition->base));
    ast_while_until_set_body(ast, &(body->base));

    // test exec
    cr_redirect_stdout();
    int res = (*(ast->base.node_exec))((struct ast_node *)ast, rt);
    fflush(stdout);
    cr_assert_stdout_eq_str("husky\n");
    cr_assert_eq(res, 0);

    // free
    (*(ast->base.node_free))((struct ast_node *)ast);
    free(ast);
    runtime_free(rt);
}
